#include <iostream>
#include "AdditionTypes.h"

using namespace std;

// consistence ws pros tin morfh tou date
bool consistentDate_str(char* date) {

    if (date == NULL)
    return true;
    
    int pavla = 0;

    while(*date) {
        if (*date == '-')
            pavla++;
        if ((*date < '0' || *date > '9') && *date != '-') //  e.g.   1/1/2011
            return false;
        *date++;
    }

    if (pavla != 2)
        return false;

    return true;
}


// consistence ws pros tous arithmous tou date
bool consistentDate(Date* date)
{
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    int day = now->tm_mday;
    int month = now->tm_mon + 1;
    int year = now->tm_year + 1900;
    Date* date_now = new Date(day, month, year);

    if (date) {
        // if (*date > *date_now) {     //anw fragma tin shmerini hmerominia
        //     delete date_now;
        //     return false;
        // }

        if ((date->get_day() > 30) || (date->get_day() <= 0)) {
            delete date_now;
            return false;
        }

        if ((date->get_month() > 12) || (date->get_month() <= 0)) {
            delete date_now;
            return false;
        }

    }

    delete date_now;
    return true;
}


int countWords (char* str)
{
    int count = 0;

    while(*str) {
        if (*str == ' ')
            count++;
        *str++;
    }
    return count + 1;
}


bool consistentName(char* name_lastName)
{
    while (*name_lastName)
    {   
        if (*name_lastName >='0' && *name_lastName <= '9')
            return false;

        *name_lastName++;
    }

    return true;
}


bool consistentNumber (char* id)
{
    while (*id)
    {
        if (*id < '0' || *id > '9')
            return false;
        *id++;
    }

    return true;
}


void printStatistics (Statistics stats)
{
    cout << "\n*********** Statistics ***********\n";
    printf("Application was loaded at: %.2fsec\n", stats.time_for_app);
    printf("%d Buckets was prepared at %.3fsec\n", stats.nBuckets, stats.time_for_buckets);
    cout << "Loaded " << stats.nRecords << " Records\n";
    cout << "Citizens " << stats.nCitizens << endl;
    cout << "Countries " << stats.nCountires << endl;
    cout << "Viruses " << stats.nViruses << endl;
    cout << "BloomFilters " <<stats.nBloomFilters << endl;
    cout << "Vaccinated Skip Lists " << stats.nVacciated_Lists << endl;
    cout << "NO Vaccinated Skip Lists " << stats.nNo_Vacciated_Lists <<"\n";
    cout << "*******************************\n\n";
}



////////////////////////////////////////////////////////////////////////
////////////////////// project 2 //////////////////////////////////////
//////////////////////////////////////////////////////////////////////




Dirs get_dir_per_Monitor(const char* input_dir, int numMonitors)
{   
    Dirs info;
    info.ndirs = NULL;
    info.dir_per_process = NULL;

    struct dirent *dir;
    struct dirent **namelist;
    int curr_monitor = 0;
    int i_dir = 0;
    int* num_dirs = (int*) malloc(sizeof(int) * numMonitors);
    int n = 0;
    char *newname = NULL;

    char*** dirs = (char***) malloc((numMonitors) * sizeof(char**));
    char** more_dirs = NULL;

    for (int i = 0; i < numMonitors; i++) {
        num_dirs[i] = 1;
        dirs[i] = NULL;
    }

    // get a sorted list with directories 
    n = scandir(input_dir, &namelist, 0, alphasort);
    if (n < 0) {
        perror("open input_dir");
        exit(1);
    }


    // make an array of the directors per process with round robin method
    // avoid the first 2 directories ( ".", ".." )
    for(int i = 2; i < n; i++) {    

  		newname = (char *)malloc(strlen(input_dir)+strlen(namelist[i]->d_name)+2);

        // make the path for every directory
        strcpy(newname, input_dir);
        strcat(newname, "/");
        strcat(newname, namelist[i]->d_name);

        more_dirs = (char**)realloc(dirs[curr_monitor], num_dirs[curr_monitor]*sizeof(char*));

        if(more_dirs != NULL) {
            dirs[curr_monitor] = more_dirs;

            i_dir = num_dirs[curr_monitor] - 1;
            dirs[curr_monitor][i_dir] = newname;
            num_dirs[curr_monitor] += 1;
        }
        else
            perror("realoc");
    

        curr_monitor = ((curr_monitor+1) >= numMonitors) ? 0 : curr_monitor+1;
    }

    for (int i = 0; i < n; i++) {
        free(namelist[i]);
    }
    free(namelist);

    info.dir_per_process = dirs;
    info.ndirs = num_dirs;

    return info;
}


char* substr(const char *src, int m, int n)
{
	if (n > strlen(src)) {
		n = strlen(src);
	}
 
    int len = n - m;
    char *dest = (char*)malloc(sizeof(char) * (len + 1));
 
    for (int i = m; i < n && (*(src + i) != '\0'); i++)
    {
        *dest = *(src + i);
        dest++;
    }
 
    *dest = '\0';
 
    return dest - len;
}



Array* get_token_array(string directories, std::string delimiter)
{
	Array* dir_arr = (Array*) malloc(sizeof(Array));
	dir_arr->arr = NULL;
	dir_arr->length = 0;

	int i_dir = 1;
	int length;
	int start = 0, end = 0;
    char* c_token;

    string dir_str = directories;
	string token;
	
	size_t pos = 0;

	while ((pos = dir_str.find(delimiter)) != string::npos) {

		token = dir_str.substr(0, pos);
        c_token = (char*)token.c_str();

		dir_arr->arr = (char**) realloc((char**)dir_arr->arr, i_dir*sizeof(token));

		dir_arr->arr[i_dir-1] = strdup((char*)c_token);

		dir_str.erase(0, pos + delimiter.length());

		i_dir++;
	}

    dir_arr->arr = (char**) realloc((char**)dir_arr->arr, i_dir*sizeof(token));
    dir_arr->arr[i_dir-1] = strdup((char*)dir_str.c_str()); 

	dir_arr->length = i_dir-1;


	return dir_arr;
}



string add_country(string line_str, char* country)
{
    string country_str(country);
    country_str = " " + country_str;

    int position = line_str.find(" "); 
    position = line_str.find(" ", position+1);  //second white space
    position = line_str.find(" ", position+1);  

    string tail = line_str.substr(position, string::npos);  
    string head = line_str.substr(0, position);   
    string record = head + country_str + tail;

    return record;
}


bool contains(char* string, char special_character)
{
    int length = strlen(string);

    for (int i = 0; i <= length; i++) {
        if (string[i] == special_character) {
            return true;
        }
    }

    return false;
}


bool contains(char* string, const char* str)
{
    std::string st(string);
    std::string s(str);

    if (st.find(s) != string::npos) {
        return true;
    }

    return false;
}


int n_directories(const char* dir)
{
    struct dirent **namelist;
    int n, counter;

    n = scandir(dir, &namelist, 0, alphasort);
    counter = n;
    if (n < 0)
        perror("scandir");
    else {
        for(int i = 0; i < n; i++) {
            // printf("%s\n", namelist[i]->d_name);
            free(namelist[i]);
       }
       free(namelist);
   }

   return counter - 2;      // subtract directories ".", ".."
}


char* get_country(char* dir) {
    char* temp_dir = strdup(dir);
    char* inputdir = strtok(temp_dir, "/");
    char* country = strdup(strtok(NULL, ""));

    free(temp_dir);
  
    return country;
}


// return the named pipe for the monitor which has this country
int get_Monitor_for_country(Dirs dirs , char* country)
{
    int monitor = 0;
    int length, i, j, flag = 0;
    char* temp_country;


    for(i = 0; i < dirs.nMonitors; i++) {
        monitor = i;
        length = dirs.ndirs[i] - 1;     //number of directories for the i monitor

        for(j = 0; j < length; j++) {

            temp_country = get_country(dirs.dir_per_process[i][j]);
            if (!strcmp(country, temp_country)) {
                flag = 1;
                free(temp_country);
                return monitor;
            }
            free(temp_country);
        }
    }

    // return -1 if dataset don't have the desire country
    return -1;
}  



void creating_logFile(string directories, int total_rq, int accepted, int rejected)
{
	Array* dir_array;
	int nCountries;
	int pid = getpid();
	dir_array = get_token_array(directories, " ");


	char** countries_log = (char**) malloc(1000* sizeof(char*));	
	memset(countries_log, '\0', 100);

	for(int i = 0; i < 1000; i++) {
        countries_log[i] = (char*)malloc(100*sizeof(char));
		memset(countries_log[i], '\0', 100);
	}

	nCountries = dir_array->length;

	// make an array with countries
	for (int i = 0; i < nCountries; i++)
	{
		char* dir = strdup(dir_array->arr[i]);		// without country
		char* _dir = strtok(dir, "/");
		char* country = strtok(NULL, "");
		strcpy(countries_log[i], country);
		free(dir);
	}

	char* logfileName = (char*) malloc(100 * sizeof(char));
	char* c_pid = (char*) malloc(100 * sizeof(char));
	char* buff = (char*) malloc(10000 * sizeof(char));
	memset(buff, '\0', 10000);


	sprintf(c_pid, "%d", pid);
	strcpy(logfileName, "logfiles/log_file.");
	strcat(logfileName, c_pid);

	for(int i = 0; i < nCountries; i++) {
		strcat(buff, countries_log[i]);
		strcat(buff, "\n");
	}

    char* total_request = (char*) malloc(sizeof(char) * 100);
    char* c_accepted = (char*) malloc(sizeof(char) * 100);
    char* c_rejected = (char*) malloc(sizeof(char) * 100);

    sprintf(total_request, "%d", total_rq);
    sprintf(c_accepted, "%d", accepted);
    sprintf(c_rejected, "%d", rejected);


    strcat(buff, "TOTAL TRAVEL REQUESTS ");
    strcat(buff, total_request);
    strcat(buff, "\n");
    strcat(buff, "ACCEPTED ");
    strcat(buff, c_accepted);
    strcat(buff, "\n");
    strcat(buff, "REJECTED ");
    strcat(buff, c_rejected);
    strcat(buff, "\n");

    free(total_request);
    free(c_accepted);
    free(c_rejected);




	FILE* file;
	file = fopen(logfileName, "w");
	if (file != NULL) {
		fputs(buff, file);
		fclose(file);
	}

    for(int i = 0; i < 1000; i++) {
        free(countries_log[i]);
    }
    free(countries_log);

	free(c_pid);
	free(logfileName);
	free(buff);

	for (int i = 0 ; i <= dir_array->length; i++) {
		free(dir_array->arr[i]);
	}
	free(dir_array->arr);
	free(dir_array);

	cout << "logfile." << pid << " was created\n" << endl;

}

