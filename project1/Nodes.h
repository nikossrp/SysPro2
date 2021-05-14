#ifndef Nodes_H_
#define Nodes_H_

#include "Record.h"

typedef struct NodeSkipList{    
    
    Record* record;
    int height;
    struct NodeSkipList** next;

}Node;


typedef struct HashTableNode{

    void* data;                  //bloomfilter, skipList, simple string (char*)
    struct HashTableNode* next;

}NodeHT;




#endif //Nodes_H_