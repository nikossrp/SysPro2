#ifndef Types_H_
#define Types_H_
#include <string.h>
#include "Date.h"

#define NUMBER_OF_COUNTRIES 195

// max number of buckets (its depends from lines in citizenRecordsFile)
#define MAXSIZE 1000      
  

// info about the viruse
typedef struct {    
    char* virusName;
    bool vaccinated;
    Date* date;

}Info;


typedef struct {
    int nBuckets;
    int nRecords;
    int nCitizens;
    int nCountires;
    int nViruses;
    int nVacciated_Lists;
    int nNo_Vacciated_Lists;
    int nBloomFilters;
    double time_for_app;
    double time_for_buckets;

}Statistics;

















// consistence ws pros tin morfh tou date
bool consistentDate_str (char* date);

// consistence ws pros tous arithmous tou date
bool consistentDate (Date* date);

// count the words in a string based on spaces
int countWords (char* str);

// Check if the name of firstname/lastName/country is consistent
bool consistentName (char* name_lastName);

//check citizenID/day,month,year/age
bool consistentNumber (char* id);

void printStatistics (Statistics stats);




/////////////////////////////////////////////////////////////////////////////
///////////////////////////// PROJECT 2 ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>




typedef struct dirs {   // for directory per monitor
    int nMonitors;
    int* ndirs;
    char*** dir_per_process;
}Dirs;



typedef struct array {
    int length;
    char** arr;

}Array;






//get with Round Rubin countries for every process
Dirs get_dir_per_Monitor(const char* input_dir, int numMonitors);
char* substr(const char *src, int m, int n);
Array* get_token_array(std::string directories, std::string delimiter);
std::string add_country(std::string line_str, char* country);
bool contains(char* string, char special_character);
bool contains(char* string, const char* str);
// count the words in a string based on spaces
int get_bloomSize(char* info);

int n_directories(const char* dir);
bool checkBit(char* a, int p);


//get monitor who manages the country (all monitors are in an array)
int get_Monitor_for_country(Dirs directories, char* country);

void creating_logFile(std::string directories, int total_request, int accepted, int rejected);


#endif