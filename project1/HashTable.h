#ifndef HashTable_H_
#define HashTable_H_
#include <iostream>
#include "Record.h"
#include "BloomFilter.h"
#include "SkipList.h"  // Nodes.h


/*  Type could be:
*   BloomFilter
*   SkipList
*   Record
*   char*
*/

class HashTable {

    private:
        void** Array;
        int nNodes;
        int buckets;
        std::string type;

    public:
        HashTable(int buckets, const std::string type);    
        HashTable();
        void* Insert(void* newItem);
        void Delete(char* virusName);
        void print();
        void* get_item(void* item);
        void* get_i_Item (int i);
        int get_nNodes () const { return nNodes; }
        void update_bloom(BloomFilter* bloom);
        std::string get_type () const { return type; }
        ~HashTable();
        unsigned long sdbm(std::string str_str);
};


#endif