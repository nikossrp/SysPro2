#include <iostream>
#include "BloomFilter.h"
#include <stdlib.h>
#include <stdio.h>

using namespace std;


BloomFilter::BloomFilter(char* virusName, int num)
{
    this->virusName = virusName;
    // this->bloomPointer = NULL;
    array = (char*) malloc(sizeof(char) * num);         // 1byte -> 8bits
    // array = new char [num];

    for ( int i = 0; i < num; i++ )    //initialize all bytes/bits to 0
    {
        array[i] = 0;
    }

    bloomSize = num * 8;      //size of bloomFilter 8bits x num
}

BloomFilter::BloomFilter(char* array, int sizeOfBloom, char* virusName)
{
    this->array = array;
    this->bloomSize = sizeOfBloom * 8;
    this->virusName = virusName;
}


BloomFilter::~BloomFilter()
{
    // delete [] array;
    free(array);
}


void BloomFilter::Insert_Record (char* str)
{
    int position = 0;
    for (int i = 0; i < K; i++)
    {
        position = (this->hash_i(str, i) % bloomSize);
        this->setBit(position);
    }
}


bool BloomFilter::check_Record (char* str)
{
    int bit = 0;
    int flag_true;
    int flag_false;

    for (int i = 0; i < K; i++)
    {
        bit = this->hash_i(str, i) % bloomSize;
        if (!this->checkBit(bit))
        {
            return false;
        }
    }
    return true;
}




bool BloomFilter::checkBit(int position)
{
    int byte = position / 8;
    int bits = byte * 8;
    int shiftn = position - bits; 
    int mask = 1 << shiftn;

    return array[byte] & mask; 
}


void BloomFilter::setBit(int position)
{
    int byte = position / 8;   //int -> 4bytes = 32bits (32-bit and 64-bit machines)    
    int bits = byte * 8;
    int shiftn = position - bits; 
    unsigned int set_bit = 1 << shiftn;

    array[byte] |= set_bit;         //set bit to one

}


BloomFilter* BloomFilter::merge_bloom(BloomFilter* bloom)
{
    for(int i = 0; i < bloomSize; i++) {
        if (bloom->checkBit(i)) {
            this->setBit(i);
        }
    }

    return this;
}


unsigned long djb2(char *str) {
	unsigned long hash = 5381;
	int c; 
	while (c = *str++) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}
	return hash;
}


unsigned long sdbm(char *str) {
	unsigned long hash = 0;
	int c;

	while (c = *str++) {
		hash = c + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}


unsigned long BloomFilter::hash_i(char *str, int i) {
	return djb2(str) + i*sdbm(str) + i*i;
}