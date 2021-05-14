#ifndef Send_Get
#define Send_Get
#include "VaccineMonitor/HashTable.h"



//Send result for a request which did the travel Monitor
// The results were written above the namePipe (every time we write bufferSize bytes)
char* send_string(char* results, int fd, int bufferSize);     //send string


// get results into buffer for a request above the namedPipe 
//(request is a string with the end of string '#')
char* get_string(char* buffer, int fd, int bufferSize);



void send_blooms(HashTable* bloomfilters, int fd, int bufferSize, int sizeOfBloom);

void get_blooms(HashTable* bloomfilters, int fd, int bufferSize, int sizeOfBloom);








#endif //Send_Get
