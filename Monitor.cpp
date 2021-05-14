#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>

#include "Dataset/Application.h"
#include "Send_Get.h"

using namespace std;


bool creat_logFile = false;
void logfile(int signo);


void new_dirs(int signo);
bool check_newDirs = false;

HashTable* get_update_blooms_app(App* app, Array oldfiles, Array* directories);
bool new_fileName(Array files, char* check_fileName);



int main(int argc, char* argv[]) {

	if (argc < 2) {
		cout << "Usage: ./Monitor myfifo\n";
		return 1;
	}

	static struct sigaction actInt_Quit, actUSR1;


	actInt_Quit.sa_handler = logfile;	
	actUSR1.sa_handler = new_dirs;	

	sigfillset(&(actInt_Quit.sa_mask));
	sigfillset(&(actUSR1.sa_mask));

	sigaction(SIGINT, &actInt_Quit, NULL);
	sigaction(SIGQUIT, &actInt_Quit, NULL);
	sigaction(SIGUSR1, &actUSR1, NULL);


	sigset_t base_mask;	// set for pending signals
	sigemptyset(&base_mask);		
	sigaddset(&base_mask, SIGINT);
	sigaddset(&base_mask, SIGQUIT);
	sigaddset(&base_mask, SIGUSR1);

	// make signals pending until application become stable
	sigprocmask(SIG_SETMASK, &base_mask, NULL);

	int nCountries = 0;
    int fd1, n, i, nwrite;
	int bufferSize = 0;

	int count = 4, temp = 0;


	char* info = (char*) malloc(2000 * sizeof(char));
	memset(info, '\0', 2000);

	char c_sizeOfBloom[100] = "";
	int sizeOfBloom = 0;
	string directories = "";
	int counter = 2;
	int bytes = 0;
	int getted_bloomSize = 1;
	int is_open = 0;
	string line_str;
	struct dirent **files;
	char* c_line;
	int total_rq = 0, accepted = 0, rejected = 0;
	Array* dir_arr = NULL;
	App* app;

	
	if ( (fd1 = open(argv[1], O_RDONLY)) == -1) {
		perror ("open");
		exit(1);
	}

	// get buferSize at first
	if (bufferSize == 0) {
		ssize_t s = read(fd1, &bufferSize, sizeof(int));
	} 

	int garbage = 0;
	if (sizeOfBloom == 0) { //garbage from bufferSize
		ssize_t s = read(fd1, &garbage, sizeof(int));	
	}

	char* msgbuf = (char*) malloc(sizeof(char) * (bufferSize+1));

	size_t size = bufferSize;
	

	// get sizeOfBloom and  directories 
	while(1) {

		memset(msgbuf, '\0', size);
		bytes = read(fd1, msgbuf, size);

		if (bytes < 0) {
			perror("read");
			exit(1);
		}
		else {

			msgbuf[bytes] = '\0';
			if (contains(msgbuf, '$')) {	//flag for end of directories
				strcat(info, msgbuf);
				break;
			}
			strcat(info, msgbuf);
		}

	}

	string info_str(info);


	// stoi will give the first number which is the size of bloomfilter
	sizeOfBloom = stoi(info_str);
	int start = info_str.find("i");		//avoid sizeOfBloom
	int end = info_str.find(" $");		//avoid the flag
	app = new App(sizeOfBloom, MAXSIZE);

	directories = info_str.substr(start, end);	// subtract '/0' and " $"
	dir_arr = get_token_array(directories, " ");

	// print directories
	// cout << "Monitor has the directories: \n";
	// for (int i = 0 ; i < dir_arr->length; i++) {
	// 	cout << dir_arr->arr[i] << endl;

	// }


	//at first we have the number of countries 
	nCountries = dir_arr->length;

	// keep all files in an 2D array, We will use that if new files entry
	Array save_files; 
	save_files.arr = NULL;
	save_files.length = 0;
	int i_file = 0;

	for(int i = 0; i < dir_arr->length; i++) {
		
		string id;


		n = scandir(dir_arr->arr[i], &files, 0, alphasort);
		if (n < 0) {
			perror("scandir");
			exit(12);
		}


		for (int j = 2; j < n; j++) {		//avoid "." and ".." direcotires

			string fileName(dir_arr->arr[i]);
			fileName = fileName + "/" + files[j]->d_name;
			
			save_files.arr = (char**) realloc (save_files.arr, (i_file+1)*sizeof(char*));
			save_files.arr[i_file++] = strdup(fileName.c_str());


			ifstream file(fileName);

			if (file) {

				while(getline(file, line_str)) {
					c_line = strdup(line_str.c_str());
					id = app->Insert_Record(c_line);
				}

			}
			else 
				perror("open file");
		}
		for (int i = 0; i < n; i++) {
			free(files[i]);
		}
		free(files);
	}

	// app->print_Records();

	save_files.length = i_file;


	int fd2;


	HashTable* bloomfilters = app->get_BloomFilters();


	if((fd2 = open(argv[2], O_WRONLY)) < 0) {	
		perror("Monitor: open for bloomFilter");
		exit(1);
	}

	// <#bytes for virusName> + <virusName> + <bloomfilter>
	send_blooms(bloomfilters, fd2, bufferSize, sizeOfBloom);


	// execute the pending signals
	sigprocmask(SIG_UNBLOCK, &base_mask, NULL);		//for 1 each time

	// for SIGINT/SIGQUIT
	if (creat_logFile) {
		creating_logFile(directories, total_rq, accepted, rejected);
		creat_logFile = false;
	}

	// for SIGUSR1
	if (check_newDirs) {
		HashTable* new_bloomfilters = get_update_blooms_app(app, save_files, dir_arr);
		send_blooms(new_bloomfilters, fd2, bufferSize, sizeOfBloom);
		check_newDirs = false;
	}



	while (1)
	{

		memset(info, '\0', 2000);

		while(1) {      // number of bytes about virusName same as nBlooms

			memset(msgbuf, '\0', bufferSize);

			bytes = read(fd1, msgbuf, bufferSize);
			if (bytes < 0) {	//interrupt from signal

				// for SIGINT/SIGQUIT
				if (creat_logFile) {
					creating_logFile(directories, total_rq, accepted, rejected);
					creat_logFile = false;
					continue;
				}

				// for SIGUSR1 - new Records
				if (check_newDirs) {
					HashTable* new_bloomfilters = get_update_blooms_app(app, save_files, dir_arr);
					send_blooms(new_bloomfilters, fd2, bufferSize, sizeOfBloom);
					check_newDirs = false;
					continue;
				}

				perror("read request");
				exit(11);
				
			}
			else {
				msgbuf[bytes] = '\0';
				
				if (contains(msgbuf, "#")) {	//flag for end of directories
					strcat(info, msgbuf);
					break;
				}

				strcat(info, msgbuf);
			}

		}


		if (contains(info, "search")) {	
			//we have a search request
			
			char* garbage = strtok(info, "$");
			char* id = strtok(NULL, "#");
			char* results = app->vaccineStatus(id);

			strcat(results, "#");
			results = send_string(results, fd2, bufferSize);
			free(results);

		}
		else {
			//serve travelRequest

			char* id = strtok(info, "$");	
			char* virus = strtok(NULL, "#");		//end of string
			
			char* results = app->vaccineStatus(id, virus);
			


			if (contains(results, "YES")) {
				accepted++;
			}
			else if (contains(results, "NO")) {
				rejected++;
			}
			total_rq++;

			strcat(results, "#");
			results = send_string(results, fd2, bufferSize);
			free(results);
		}
	}
	



	// free memory is not neccassary when travelMonitor exit kill all children with SIGKILL, 
	//as we know SIGKILL is the best deallocation
	free(info);

	for(int i = 0; i < save_files.length; i++)
	{
		free(save_files.arr[i]);
	}

	for (int i = 0 ; i <= dir_arr->length; i++) {
		free(dir_arr->arr[i]);
	}
	free(dir_arr->arr);
	free(dir_arr);
	free(msgbuf);

	delete app;

    return 0;
}




void logfile(int signo)
{
	creat_logFile = true;		
}


void new_dirs(int signo)
{	
	check_newDirs = true;
}




HashTable* get_update_blooms_app(App* app, Array oldfiles, Array* dir_array)
{	
	struct dirent **files;
	string line_str;
	string id;
	char* c_line;
	int n;

	for(int i = 0; i < dir_array->length; i++) {

		n = scandir(dir_array->arr[i], &files, 0, alphasort);
		if (n < 0) {
			perror("scandir");
			exit(12);
		}

		for(int j = 2; j < n; j++) {	//find the new files
			string fileName(dir_array->arr[i]);
			fileName = fileName + "/" + files[j]->d_name;

			if (new_fileName(oldfiles, (char*)fileName.c_str())) {
				ifstream file(fileName);
					
				if (file) {
					while(getline(file, line_str)) {

						c_line = strdup(line_str.c_str());

						id = app->Insert_Record(c_line);
					}
				}
				else {
					perror("open file - get_update_blooms_app");
					exit(15);

				}
			}

	}

	for (int i = 0; i < n; i++) {
		free(files[i]);
	}
		free(files);
	}	

	return app->get_BloomFilters();

}


bool new_fileName(Array files, char* check_fileName)
{
	for(int i = 0; i < files.length; i++) {
		if (!strcmp(files.arr[i], check_fileName)) {
			return false;
		}
	}

	return true;
}






