#include "Send_Get.h"

using namespace std;


char* send_string(char* results, int fd, int bufferSize)
{
    int fd1 = fd, length;
    int nwrite;
    char* start_str = results;
    char* msgbuf = (char*) malloc (sizeof(char) * (bufferSize+1));

    length = strlen(results);


    while(length > 0) {
        memset(msgbuf, '\0', bufferSize);

        if (length <= bufferSize) {
            strcpy(msgbuf, results);

            if((nwrite = write(fd1, msgbuf, length)) == -1) {
                perror("Eror in write string1");
                exit(1);
            }
        }
        else {

            memcpy(msgbuf, results, bufferSize * sizeof(char));
            if((nwrite = write(fd1, msgbuf, bufferSize)) == -1) {
                perror("Eror in write string2");
                exit(1);
            }

        }

            
        results += bufferSize;
        length -= bufferSize;
    }

    free(msgbuf);

    return start_str;
}



char* get_string(char* info, int fd, int bufferSize)
{
    int fd1 = fd;
    ssize_t bytes;
    char* msgbuf = (char*) malloc(sizeof(char) * (bufferSize+1));

    memset(info, '\0', 2000);

    while(1) {      // number of bytes about virusName same as nBlooms

        memset(msgbuf, '\0', bufferSize);

        bytes = read(fd1, msgbuf, bufferSize);
        if (bytes < 0) {
            perror("get_string - read1");
            exit(1);
        }
        else {
            msgbuf[bytes] = '\0';
            
            if (contains(msgbuf, "#")) {	//flag for end of string
                strcat(info, msgbuf);
                break;
            }
            
            strcat(info, msgbuf);

        }

    }
    free(msgbuf);

    return info;
}


// send bloomFilters (from Monitor to TravelMonitor)
void send_blooms(HashTable* bloomfilters, int fd, int bufferSize, int sizeOfBloom)
{
	int fd2 = fd;
    char* msgbuf = (char*) malloc(sizeof(char) * bufferSize);
	ssize_t nwrite, size = bufferSize;


	for (int i = 0; i < bloomfilters->get_nNodes(); i++) {

		BloomFilter* bloom = (BloomFilter*)bloomfilters->get_i_Item(i);
		char* bloomArray = bloom->get_BloomFilter();
		int bytesBloom = sizeOfBloom;				//get the size converted in bytes
		char* send = (char*)malloc(100* sizeof(char));
		char* f = send;
		int nBlooms = bloomfilters->get_nNodes();
		char c_nBlooms[10] = "";
		char* c_virusName = bloom->get_virusName();
		int vNlength;
		int length;
		char c_length[10] = "";

		memset(send, '\0', 100);

		sprintf(c_nBlooms, "%d", nBlooms);

		// put the number of bloomfilters
		if (i == 0) {
			strcpy(send, c_nBlooms);
			strcat(send, "$");
		}
		
		//save the number of bytes for virus name
		vNlength = strlen(c_virusName);		
		sprintf(c_length, "%d", vNlength);
		strcat(send, c_length);
		strcat(send, "$");

		//save the virusName
		strcat(send, c_virusName);


		length = strlen(send);
		// send virusName
		while(length > 0) {

			memset(msgbuf, '\0', bufferSize);

			strcpy(msgbuf, send);

			if (bufferSize > length) {

				strcpy(msgbuf, send);

				if((nwrite=write(fd2, msgbuf, length)) == -1) {
					perror("Error in Writing");
					exit(2);
				}

			}else if ((nwrite = write(fd2, msgbuf, size)) == -1) {

				perror("Eror in write virusName");
				exit(1);
			}

			length -= bufferSize;
			send += bufferSize;
		}



		//send bloomFilter
		while(bytesBloom > 0) {

			memset(msgbuf, '\0', bufferSize);


			if (bufferSize > bytesBloom) {

				memcpy(msgbuf, bloomArray, bytesBloom * sizeof(char));

				if((nwrite=write(fd2, bloomArray, bytesBloom)) == -1) { //allaxe me fd2
					perror("Error in Writing");
					exit(2);
				}
				
			}
			else  {

				memcpy(msgbuf, bloomArray, bufferSize * sizeof(char));

				if((nwrite = write(fd2, bloomArray, size)) == -1) {
					perror("Eror in write virusName");
					exit(1);
				}
			}
			
			bloomArray += nwrite;		//move pointer to next bytes
			bytesBloom -= nwrite;

		}


		free(f);
	}

}



void get_blooms(HashTable* bloomfilters, int fd, int bufferSize, int sizeOfBloom)
{
    int fd2 = fd;
    // char msgbuf[bufferSize+1];
    char* msgbuf = (char*) malloc(sizeof(char) * (bufferSize+1));
    ssize_t bytes;
    int bVirusName = 0, temp_size;
    int b = bufferSize;
    int nBlooms = 0;
    char* virusName;
    char virusName_b[100];
    char c_bVirusName[10] = "";
    char c_Blooms[10] = "";

    // get number of blooms 
    //(we have no other option here we need to read one by one bytes)
    while(1) {

        bytes = read(fd2, msgbuf, 1);
        if (bytes < 0) {
            perror("get_blooms - read");
            exit(1);
        }
        else {
            msgbuf[bytes] = '\0';

            if (contains(msgbuf, '$')) {	//flag for end of directories
                strcat(c_Blooms, msgbuf);
                break;
            }

            strcat(c_Blooms, msgbuf);
        }
    }

    nBlooms = atoi(c_Blooms);



    while(nBlooms--) {

        bVirusName = 0;
        temp_size = sizeOfBloom;

        memset(msgbuf, '\0', bufferSize);
        memset(c_bVirusName, '\0', 10);     // change to dynamic please
        memset(virusName_b, '\0', 100);



        while(1) {      // number of bytes about virusName same as nBlooms

            memset(msgbuf, '\0', bufferSize);

            bytes = read(fd2, msgbuf, 1);
            if (bytes < 0) {
                perror("read bytes for virusName");
                exit(1);
            }
            else {
                msgbuf[bytes] = '\0';

                if (contains(msgbuf, '$')) {	//flag for end of directories
                    strcat(c_bVirusName, msgbuf);
                    break;
                }

                strcat(c_bVirusName, msgbuf);
            }

        }

        bVirusName = atoi(c_bVirusName);



        while(bVirusName > 0) {     // get virusName
            memset(msgbuf, '\0', bufferSize);

            if (bufferSize > bVirusName) {
                bytes = read(fd2, msgbuf, bVirusName * sizeof(char));
                if (bytes < 0) {
                    perror("read virusName");
                    exit(1);
                }
                else {
                    msgbuf[bytes] = '\0';
                    strcat(virusName_b, msgbuf);
                }
            }
            else {
                bytes = read(fd2, msgbuf, bufferSize * sizeof(char));
                if (bytes < 0) {
                    perror("read virusName");
                    exit(1);
                }
                else {
                    msgbuf[bytes] = '\0';
                    strcat(virusName_b, msgbuf);
                }
            }

            bVirusName -= bufferSize;
        }

        virusName = strdup(virusName_b);

        char* bloomArray = (char*)malloc(sizeof(char) * sizeOfBloom);

        char* array = bloomArray;

        // get the array of bits (bloomfilter)
        while(temp_size > 0) {     

            memset(msgbuf, '\0', bufferSize);

            if (bufferSize > temp_size) {
                bytes = read(fd2, msgbuf, temp_size);
                if (bytes < 0) {
                    perror("read array fo bits");
                    exit(1);
                }
                else {

                    msgbuf[bytes] = '\0';
                    memcpy(bloomArray, msgbuf, sizeof(char) * temp_size);
                    bloomArray += bytes;
                    break;
                }
            }
            else {

                bytes = read(fd2, msgbuf, bufferSize);
                if (bytes < 0) {
                    perror("read array of bits");
                    exit(1);
                }
                else {
                    msgbuf[bytes] = '\0';

                    memcpy(bloomArray, msgbuf, sizeof(char) * bufferSize);
                    bloomArray += bytes;
                }
            }

            temp_size -= bytes;
        }


        BloomFilter* bloom = new BloomFilter(array, sizeOfBloom, virusName);

        // check if already exist the bloomFilter
        BloomFilter* new_bloom = (BloomFilter*) bloomfilters->get_item(virusName);
        if (new_bloom != NULL) {  
            new_bloom->merge_bloom(bloom);

            free(virusName);
            delete bloom;
            continue;
        }

        bloomfilters->Insert(bloom);
    }

    free(msgbuf);
}