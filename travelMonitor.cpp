#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include "VaccineMonitor/Application.h"
#include "TravelReuests.h"
#include "Send_Get.h"

#define Monitor "./Monitor"
#define Myfifos "/tmp/"

using namespace std;


bool ex = false;
void exitt(int signo) {
    ex = true;
}


bool mourning = false;
void handlSigChild(int signo) {
    //dead Monitor detected
    mourning = true;
}


void replaceChild (
    size_t bufferSize, int sizeOfBloom, pid_t* child_pids,
    Dirs dirs, string* Wpipe, string* Rpipe, int* Rfds, int* Wfds,
    HashTable* bloomfilters
);



int main (int argc, char* argv[]) {

    if (argc != 9) {
        cout << "Failure usage:./travelMonitor –m numMonitors -b bufferSize -s sizeOfBloom -i input_dir" << endl;
        exit(EXIT_FAILURE);
    }



    int sizeOfBloom = 0;
    int numMonitors = 0;
    char* input_dir = NULL;
    int check_bufferSize = 0;         

    for (int i = 0; i < argc; i++) {

        if (!strcmp(argv[i], "-s")) {
            sizeOfBloom = atoi(argv[++i]);
            if (sizeOfBloom <= 0 ) {
                cout << "Error: sizeOfBloom shouldn't be negative or zero\n";
                exit(EXIT_FAILURE);
            }
        }

        if (!strcmp(argv[i], "-b")) {
            check_bufferSize = atoi(argv[++i]);
            if (check_bufferSize <= 0 ) {
                cout << "Error: bufferSize shouldn't be negative or zero\n";
                exit(EXIT_FAILURE);
            }
        }

        if (!strcmp(argv[i], "-m")) {
            numMonitors = atoi(argv[++i]);
            if (numMonitors <= 0 ) {
                cout << "Error at number of Monitors, it shouldn't be negative or zero\n";
                exit(EXIT_FAILURE);
            }
        }


        if (!strcmp(argv[i], "-i")) {
            input_dir = strdup(argv[++i]);
        }

    }

    ssize_t bufferSize = check_bufferSize;

    int n_dir = n_directories(input_dir);

    //if given more monitors than directories
    if (n_dir < numMonitors) {
        cout << "Number of monitors is more than number of directories : " << n_dir << endl;
        numMonitors = n_dir;

    }


    char* myfifo1 = NULL;
    char* myfifo2 = NULL;
    int length_arr = 0;
    string i_str = "", j_str = "";

    Dirs dirs = get_dir_per_Monitor(input_dir, numMonitors);  //get with Round Rubin countries for every monitor
    dirs.nMonitors = numMonitors;

    int fd2 = 0, fd1 = 0;
    string* Wpipe = NULL;
    string* Rpipe = NULL;
    int Rfds[numMonitors];
    int Wfds[numMonitors];
    string all_directories = "";

    char* msgbuf = (char*) malloc(sizeof(char) * (bufferSize+1));
    pid_t child_pids[numMonitors];
    ssize_t  nwrite = 0; 


    static struct sigaction actMourning;
    actMourning.sa_handler = handlSigChild;     //act when a dead child detect
    sigfillset(&(actMourning.sa_mask));
    sigaction(SIGCHLD, &actMourning, NULL);

    static struct sigaction actInt_Quit;
    actInt_Quit.sa_handler = exitt;         //act to ctrl + c / ctr + z
    sigfillset(&(actInt_Quit.sa_mask));
    sigaction(SIGINT, &actInt_Quit, NULL);




    sigset_t sigExit;

    sigemptyset(&sigExit);
    sigaddset(&sigExit, SIGINT);
    sigaddset(&sigExit, SIGQUIT);





    // 2 pipes for every process one for read and one for write
    Rpipe = new string[numMonitors];
    Wpipe = new string[numMonitors];
    HashTable* bloomfilters = new HashTable(MAXSIZE, "BloomFilter");

    string myfifos = Myfifos;

    //make an array with names of pipes
    for(int i = 0; i < numMonitors; i++) {
        i_str = to_string(i);
        Wpipe[i] = myfifos + "myfifo" + i_str + "_" + i_str;

        j_str = to_string(i+1);
        Rpipe[i] = myfifos + "myfifo" + i_str + "_" + j_str;
    }




    // build named pipes
    for(int i = 0; i < numMonitors; i++) {

        if ( mkfifo(Wpipe[i].c_str(), 0666) == -1 ){  
            if ( errno!=EEXIST ) {
                perror("server: mkfifo"); exit(6); 
            };
        }

        if ( mkfifo(Rpipe[i].c_str(), 0666) == -1 ){  
            if ( errno!=EEXIST ) {
                perror("server: mkfifoi_i"); exit(6); 
            };
        }
    }

    
    for (int i = 0; i < numMonitors; i++) {


        myfifo1 = (char*)Wpipe[i].c_str();
        myfifo2 = (char*)Rpipe[i].c_str();

        // keep all pids from children in an array, we need that for replacement children
        if ((child_pids[i] = fork()) == 0) {  
            //child process

            execlp(Monitor, "Monitor", myfifo1, myfifo2, NULL);
            perror("execlp");
            exit(1);

        }
        else if (child_pids[i] != 0) {    //parent process

            char msgbuf[bufferSize+1];
            char* str = NULL;
            int count = 4;
            int length;


            if ( (Wfds[i]=open(myfifo1, O_WRONLY )) < 0) { 
                perror("open"); 
                exit(1); 
            }

            fd1 = Wfds[i];

            // send bufferSize
            if ((nwrite=write(fd1, &bufferSize, sizeof(bufferSize))) == -1) { 
                perror("Error in Writing"); 
                exit(2); 
            }

            //   send sizeOfBloom                   \|/
            char* bloomSize = (char*) malloc(100 * sizeof(char));
            sprintf(bloomSize, "%d", sizeOfBloom);

            //monitor will find the first latter then 
            //it will tokenize bloomsize + directories
            send_string(bloomSize, fd1, bufferSize);   

            free(bloomSize);



            // send the directories for Monitor i
            int number_of_dirs = dirs.ndirs[i]-1;

            for (int j = 0; j < number_of_dirs; j++) {
                char* s = (char*) malloc(100 * sizeof(char));
                strcpy(s, dirs.dir_per_process[i][j]);   // get the j dir to send from pipe 
                string str(s);
                all_directories += s;
                all_directories += " ";

                strcat(s, " ");
                send_string(s, fd1, bufferSize);

                free(s);
            }


            strcpy(msgbuf, "$");        //flag end of directories
            if ((nwrite=write(fd1, msgbuf, strlen(msgbuf))) == -1) { 
                perror("Error in Writing"); 
                exit(2); 
            }

        }
    }



    //~~~~~~~~~ get bloomFilters!! (?) ~~~~~~~~~~~~

    /*           Protocol
    * At first we get the number of bytes for virusName
    * Read virus name with bufferSize bytes each time.
    * Read bloomFilter (must read sizeOfBloom) bytes
    */
    struct pollfd pfds[numMonitors];



    // open all pipes then wait for the first response
    //open with blocked mood to synchronize the writers / Monitors
    for (int j = 0; j < numMonitors; j++) {

        myfifo2 = (char*)Rpipe[j].c_str();

        pfds[j].fd = open(myfifo2, O_RDONLY, 0);
        if (pfds[j].fd == -1) {
            perror("poll open");
            exit(10);
        }

        Rfds[j] = pfds[j].fd;

        pfds[j].events = POLLIN;
    }


    // get bloomfilters from the faster Monitor each time
    int answers = numMonitors;
    while(answers > 0) {
        int ready;
        ready = poll(pfds, numMonitors, -1);
        if (ready == -1) {
            perror("poll timeout");
            exit(11);
        }

        for(int i = 0; i < numMonitors; i++) {

            if (pfds[i].revents == POLLIN) {
                get_blooms(bloomfilters, pfds[i].fd, bufferSize, sizeOfBloom);
                answers--;
            }
        }

    }






    char* info = (char*) malloc(2000 * sizeof(char));
    char* start_info = info;
    char* myfifo = NULL;
    int length = 0;
    memset(msgbuf, '\0', bufferSize);
    string line_str = "";
    char* c_line = NULL;
    int nWords = 0;
    
    TravelRequests* travelStats = new TravelRequests(MAXSIZE);

    int total_request = 0;
    int accepted = 0;
    int rejected = 0;


    cout << "*********** ENTER COMMANDS ***********\n";

    while(1) {
        
        memset(info, '\0', 2000);
        cout << endl;
        getline(cin, line_str);
        c_line = strdup(line_str.c_str());
        nWords = countWords(c_line);
 
        if (ex) {
            // SIGINT/SIGQUIT detect
            for(int i = 0; i < numMonitors; i++) {
                kill(child_pids[i], SIGKILL);

                if(close(Wfds[i]) == -1) {
            		perror("Monitor: close");
                    exit(2);
                }

                if(close(Rfds[i]) == -1) {
            		perror("Monitor: close");
                    exit(2);
                }
            }

            creating_logFile(all_directories, total_request, accepted, rejected);
            free(c_line);
            break;
        }


        if (mourning) {     
            //dead Monitor detect
            free(c_line);
            
            replaceChild (
                bufferSize, sizeOfBloom, child_pids, dirs,
                Wpipe, Rpipe, Rfds, Wfds,
                bloomfilters
            );

            mourning = false;
            cin.clear();
            continue;
        }



        char* command = strtok(c_line, " ");


        if (line_str.empty()) {      // note: getline not read '\n'
            free(c_line);
            continue;
        }


        if (!strcmp(command, "/exit")) {
            for(int i = 0; i < numMonitors; i++) {
                kill(child_pids[i], SIGKILL);

                if(close(Wfds[i]) == -1) {
            		perror("Monitor: close");
                    exit(2);
                }

                if(close(Rfds[i]) == -1) {
            		perror("Monitor: close");
                    exit(2);
                }
            }
            creating_logFile(all_directories, total_request, accepted, rejected);
            free(c_line);
            break;
        }


        if ((strcmp(command, "/exit") && strcmp(command, "/travelRequest") &&
            strcmp(command, "/travelStats") && strcmp(command, "/addVaccinationRecords") &&
            strcmp(command, "/searchVaccinationStatus") && strcmp(command, "/list-nonVaccinated-Persons")) || nWords == 1) {
            cout << "ERROR COMMAND...type one of the following commands:\n";
            cout << "/travelRequest citizenID date countryFrom countryTo virusName\n";
            cout << "/travelStats virusName date1 date2 [country]\n";
            cout << "/addVaccinationRecords country\n";
            cout << "/searchVaccinationStatus citizenID\n";
            cout << "/exit" << endl;
            free(c_line);
            continue;
        }






        //before exit travelMonitor must give answer to the query
        sigprocmask(SIG_SETMASK, &sigExit, NULL);

        // /travelRequest citizenID date countryFrom countryTo virusName
        if (!strcmp(command, "/travelRequest")) {
            if (nWords != 6) {
                free(c_line);
                cout << "ERROR COMMAND\n";
                cout << "Try: /travelRequest citizenID date countryFrom virusName\n";
                continue;
            }


            Date* travel_date;
            char* id = strtok(NULL, " ");
            char* c_date = strtok(NULL, " ");
            char* countryFrom = strtok(NULL, " ");
            char* countryTo = strtok(NULL, " ");
            char* virusName = strtok(NULL, " ");

            int day = atoi( strtok(c_date, "-") );
            int month = atoi( strtok(NULL, "-") );
            int year = atoi( strtok(NULL, "-") );

            travel_date = new Date(day, month, year);


            BloomFilter* bvirus = (BloomFilter*)bloomfilters->get_item(virusName);

            int monitor = get_Monitor_for_country(dirs, countryFrom);

            if (monitor == -1) {
                cout << "Aplication hasn't records for country: " << countryFrom << endl;
                free(c_line);
                delete travel_date;
                continue;
            }

            total_request++;

            if (bvirus != NULL) {
                int ck = bvirus->check_Record(id);

                if (ck == 0) {
                    travelStats->InsertRequest(virusName, countryTo, ck, travel_date);
                    cout << "REQUEST REJECTED – YOU ARE NOT VACCINATED\n";
                    rejected++;

                }
                else {

                    memset(info, '\0', 2000);
                    strcpy(info, id);
                    strcat(info, "$");          //end of id
                    strcat(info, virusName);
                    strcat(info, "#");          //end of string



                    // send request 
                    info = send_string(info, Wfds[monitor], bufferSize);


                    //get result in info
                    info = get_string(info, Rfds[monitor], bufferSize);

                    if (contains(info, "None")) {
                        cout << "No match id " << id << " with country " <<  countryFrom << endl;
                        free(c_line);
                        delete travel_date;
                        continue;
                    }


                    char* answer = strdup(info);
                    char* yes_no = strtok(answer, " ");
                    memset(info, '\0', 2000);   

                    //checks if date is less than 6 months before the desired date
                    if (!strcmp(yes_no, "YES")) {
                        int day = atoi( strtok(NULL, "-") );
                        int month = atoi( strtok(NULL, "-") );
                        int year = atoi( strtok(NULL, "-") );
                        Date* vacc_date = new Date(day, month, year);

                        int acc = vacc_date->accepted_date6Months(travel_date);
                        if ( acc == 1) {  
                            travelStats->InsertRequest(virusName, countryTo, acc, travel_date);
                            cout << "REQUEST ACCEPTED – HAPPY TRAVELS\n";
                            accepted++;
                        }
                        else if (acc == 0) {
                            travelStats->InsertRequest(virusName, countryTo, acc, travel_date);
                            cout << "REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL " << *travel_date << endl;
                            rejected++;
                        }

                        delete vacc_date;
                    }
                    else {
                        travelStats->InsertRequest(virusName, countryTo, false, travel_date);
                        cout << "REQUEST REJECTED – YOU ARE NOT VACCINATED\n";
                        rejected++;
                    }

                    free(answer);
                }
            }
            else {
                cout << "No records for " << virusName << endl;
            }

            delete travel_date;

        }


    //  /travelStats virusName date1 date2 [country]
        if (!strcmp(command, "/travelStats")) {
            if (nWords != 5 && nWords != 4) {
                free(c_line);
                cout << "ERROR COMMAND\n";
                cout << "Try: /travelStats virusName date1 date2 [country]\n";
                continue;
            }

            char* virusName = strtok(NULL, " ");
            char* c_date1 = strtok(NULL, " ");
            char* c_date2 = strtok(NULL, " ");

            if (total_request > 0) {

                BloomFilter* virus = (BloomFilter*)bloomfilters->get_item(virusName);

                if (virus != NULL) {

                    if (nWords == 5) {

                        char* countryTo = strtok(NULL, "");
                        int d1 = atoi( strtok(c_date1, "-") );
                        int m1 = atoi( strtok(NULL, "-") );
                        int y1 = atoi( strtok(NULL, "") );

                        Date* date1 = new Date(d1, m1, y1);

                        int d2 = atoi( strtok(c_date2, "-") );
                        int m2 = atoi( strtok(NULL, "-") );
                        int y2 = atoi( strtok(NULL, "") );

                        Date* date2 = new Date(d2, m2, y2);

                        // travel requests for countryTo
                        travelStats->TravelStats(virusName, date1, date2, countryTo);

                        delete date1;
                        delete date2;
                    }
                    else if (nWords == 4) {

                        int d1 = atoi( strtok(c_date1, "-") );
                        int m1 = atoi( strtok(NULL, "-") );
                        int y1 = atoi( strtok(NULL, "") );

                        Date* date1 = new Date(d1, m1, y1);

                        int d2 = atoi( strtok(c_date2, "-") );
                        int m2 = atoi( strtok(NULL, "-") );
                        int y2 = atoi( strtok(NULL, "") );

                        Date* date2 = new Date(d2, m2, y2);

                        // travel requests for all countries together
                        travelStats->TravelStats(virusName, date1, date2);

                        //travel requests per country
                        // travelStats->TravelStats_per_country(virusName, date1, date2);
    
                        delete date1;
                        delete date2;
                    }

                }
                else {
                    cout << "No records for " << virusName << endl;
                }
            }

        }

        // /addVaccinationRecords country 
        if (!strcmp(command, "/addVaccinationRecords")) {
            if (nWords != 2) {
                free(c_line);
                cout << "ERROR COMMAND\n";
                cout << "Try: /addVaccinationRecords country\n";
                continue;
            }

            char* country = strtok(NULL, "");

            //get the child who manages the country
            int monitor = get_Monitor_for_country(dirs, country);
            pid_t child = child_pids[monitor];

            kill(child, SIGUSR1);

            get_blooms(bloomfilters, Rfds[monitor], bufferSize, sizeOfBloom);
            cout << "Dataset updated\n";

        }



        // /searchVaccinationStatus citizenID
        if (!strcmp(command, "/searchVaccinationStatus")) {
            if (nWords != 2) {
                free(c_line);
                cout << "ERROR COMMAND\n";
                cout << "Try: /searchVaccinationStatus citizenID\n";
                continue;
            }

            //send citizenID to all Monitors
            char* citizenID = strtok(NULL, "");
            int get_results = 0;
            char* search_id = (char*) malloc(100 * sizeof(char));
            strcpy(search_id, "search$");
            strcat(search_id, citizenID);
            strcat(search_id, "#");

            char* results = (char*) malloc(sizeof(char) * 1000);
            memset(results, '\0', 1000);

            for(int j = 0; j < numMonitors; j++) {

                send_string(search_id, Wfds[j], bufferSize);
                pfds[j].fd = Rfds[j];
                pfds[j].events = POLLIN;
            }   


            // using poll to detect message from the right child
            int num_answers = numMonitors;
            while(num_answers > 0) {
                int ready;
                ready = poll(pfds, numMonitors, 300);
                if (ready == -1) {
                    perror("poll");
                    exit(11);
                }
                if (ready == 0) {       
                    // poll time out
                    break;
                }

                for(int k = 0; k < numMonitors; k++) {
                    if (pfds[k].revents == POLLIN) {
                        memset(info, '\0', 2000);
                        info = get_string(info, pfds[k].fd, bufferSize);

                        if (!contains(info, "None")) {
                            strcpy(results, info);
                            get_results = 1;
                        } 
                        num_answers--;
                    }
    
                }
            }

            if (contains(results, "#")) {
                char* output = strtok(results, "#");
                cout  << output << endl;
            }
            else {
                cout << "No records for citizen id: " << citizenID << endl;
            }

            free(search_id);
            free(results);
        }


        // unblock exit signal after serve requests
        sigprocmask(SIG_UNBLOCK, &sigExit, NULL);


        free(c_line);
    }







/////////////////////////////////////////////////////////////////////////
///////////////////////// free structs/classes /////////////////////////
////////////////////////////////////////////////////////////////////////

    // free virusName / array for every bloomfilter
    for (int i = 0; i < bloomfilters->get_nNodes(); i++) {
        BloomFilter* bloom = (BloomFilter*)bloomfilters->get_i_Item(i);
        free(bloom->get_virusName());
    }

    // free dirs
    for (int i = 0; i < numMonitors; i++) {
        length_arr = dirs.ndirs[i] - 1;

        for (int j = 0; j < length_arr; j++) {
            free(dirs.dir_per_process[i][j]);
        }
        free(dirs.dir_per_process[i]);
    }
    free(dirs.dir_per_process);
    free(dirs.ndirs);

    free(start_info);
    free(input_dir);

    delete bloomfilters;
    delete [] Rpipe;
    delete [] Wpipe;
    delete travelStats;
    free(msgbuf);

    cout << "\nTravelMonitor terminated successfully\n\n\n";

    return 0;
}






void replaceChild ( 
    size_t bufferSize, int sizeOfBloom, pid_t* child_pids,
    Dirs dirs, string* Wpipe, string* Rpipe, int* Rfds, int* Wfds,
    HashTable* bloomfilters 
)
{
    int status;
    int nwrite, bytes;
    int childpid;
    char msgbuf[bufferSize+1];

    if ((childpid=waitpid(-1, &status, WNOHANG)) != -1) {
        cout << "Monitor with pid: " << childpid << " died\n";
    }


    // find the monitor that die
    int monitor = 0;

    for(int i = 0; i < dirs.nMonitors; i++)
    {
        if (childpid == child_pids[i]) {
            monitor = i;
            break;
        }
    }  

    if(close(Wfds[monitor]) == -1) {
        perror("Monitor: close");
        exit(2);
    }

    if(close(Rfds[monitor]) == -1) {
        perror("Monitor: close");
        exit(2);
    }

    char* myfifo1 = (char*) Wpipe[monitor].c_str();
    char* myfifo2 = (char*) Rpipe[monitor].c_str();


    // replace the dead monitor pid and fds
    // send all necessary information to the new Monitor to build dataset for directories 
    if ((child_pids[monitor] = fork()) == 0) {

        execlp(Monitor, "Monitor", myfifo1, myfifo2, NULL);
        perror("execlp");
        exit(1);

    }
    else if (child_pids[monitor] != 0) {

        cout << "Monitor " << childpid << " replaced with Monitor " << child_pids[monitor] << endl;

        if ( (Wfds[monitor]=open(myfifo1, O_WRONLY )) < 0) { 
            perror("open"); 
            exit(1); 
        }

        int fd1 = Wfds[monitor];

        // send bufferSize
        if ((nwrite=write(fd1, &bufferSize, sizeof(bufferSize))) == -1) { 
            perror("Error in Writing"); 
            exit(2); 
        }

        char* bloomSize = (char*) malloc(100 * sizeof(char));

        sprintf(bloomSize, "%d", sizeOfBloom);

        send_string(bloomSize, fd1, bufferSize);

        free(bloomSize);

        int number_of_dirs = dirs.ndirs[monitor]-1;

        for (int j = 0; j < number_of_dirs; j++) {
            char* s = (char*) malloc(100 * sizeof(char));
            strcpy(s, dirs.dir_per_process[monitor][j]);   // get the j dir to send from pipe 
            string str(s);

            strcat(s, " ");
            send_string(s, fd1, bufferSize);

            free(s);
        }

        strcpy(msgbuf, "$");        //flag end of directories
        if ((nwrite=write(fd1, msgbuf, strlen(msgbuf))) == -1) { 
            perror("Error in Writing"); 
            exit(2); 
        }

        if ( (Rfds[monitor]=open(myfifo2, O_RDONLY )) < 0) { 
            perror("open"); 
            exit(1); 
        }

        int fd2 = Rfds[monitor];

        // get bloomfilters 
        //(it isn't necessary that bcs we have already all bloomfilters at travelMonitor
        // but somehow monitor must get out from send_blooms)
        get_blooms(bloomfilters, fd2, bufferSize, sizeOfBloom);

    }


}


