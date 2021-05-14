#include "HashTable.h"
#include <string.h>
#include <string>
#include <iostream>

using namespace std;

HashTable::HashTable(int buckets, const string type)
{
    this->buckets = buckets;
    this->type = type;
    this->nNodes = 0;

    Array = (void**)malloc (sizeof(NodeHT*) * buckets);

    for (int i = 0; i < buckets; i++)
        Array[i] = NULL;
}


HashTable::~HashTable()
{
    NodeHT* temp;
    NodeHT* fr;
    for (int i = 0; i < buckets; i++) {
        temp = (NodeHT*)Array[i];
        fr = temp;
        while (temp) {
            temp = temp->next;

            if (!type.compare("BloomFilter")) {
                delete (BloomFilter*)fr->data;
                free (fr);
            }
            else if (!type.compare("SkipList")) {
                delete (SkipList*)fr->data;
                free (fr);
            }
            else if (!type.compare("Record")) {
                delete (Record*)fr->data;
                free (fr);
            }
            else if (!type.compare("String")) {
                free( (char*)fr->data );
                free (fr);
            }

            fr = temp;
        }
    }

    free (Array);
}


void* HashTable::Insert(void* item)
{   
    nNodes++;

    if (!type.compare("Record")) {

        string id = ((Record*)item)->get_id();
        int hashValue = this->sdbm(id) % buckets;

        NodeHT* node = (NodeHT*) malloc (sizeof (NodeHT));
        node->data = (Record*)item;
        node->next = NULL;

        NodeHT* temp = (NodeHT*)Array[hashValue];

        while (temp && temp->next != NULL) {  //phgaine sto telos tis listas
            temp = temp->next;
        }

        if (temp)
            temp->next = node;
        else 
            Array[hashValue] = node;

        return node->data;
    }  
    else if (!type.compare("BloomFilter")) {

        char* c_virusName = ((BloomFilter*)item)->get_virusName();
        string virusName(c_virusName, strlen(c_virusName)+1);

        int hashValue = this->sdbm(virusName) % buckets;

        NodeHT* node = (NodeHT*) malloc (sizeof (NodeHT));
        node->data = (BloomFilter*)item;
        node->next = NULL;

        NodeHT* temp = (NodeHT*)Array[hashValue];

        while (temp && temp->next != NULL) { 
            temp = temp->next;
        }

        if (temp)
            temp->next = node;
        else {
            Array[hashValue] = node;
        }
        
        return node->data;
    } 
    else if (!type.compare("SkipList")) {

        char* c_virusName = ((SkipList*)item)->get_virusName();
        string virusName(c_virusName, strlen(c_virusName)+1);
        int hashValue = this->sdbm(virusName) % buckets;

        NodeHT* node = (NodeHT*) malloc (sizeof (NodeHT));
        node->data = (SkipList*)item;
        node->next = NULL;

        NodeHT* temp = (NodeHT*)Array[hashValue];
        while (temp && temp->next != NULL) { 
            temp = temp->next;
        }

        if (temp)
            temp->next = node;
        else
            Array[hashValue] = node;

        return node->data;
    }      
    else if (!type.compare("String")) {

        string str ((char*)item, strlen((char*)item)+1);
        int hashValue = this->sdbm(str) % buckets;

        NodeHT* node = (NodeHT*) malloc (sizeof(NodeHT));
        node->data = strdup((char*)item);
        node->next = NULL;

        NodeHT* temp = (NodeHT*) Array[hashValue];
        while (temp && temp->next != NULL) {
            temp = temp->next;
        }

        if (temp) {
            temp->next = node;
        }
        else Array[hashValue] = node;

        return node->data;
    }

    return NULL;
}


void HashTable::Delete(char* virusName)
{
    return;
}





/* Mporei na epistrepsei enan pointer se:
*   SkipList me virus name item   
*   Bloom filter me virus name item
*   Record me to citizenID item 
*   Virus name
*   Country name
*/
void* HashTable::get_item(void* item)
{
    //epistrefei to record me to idio id
    if (!type.compare("Record")) {
        string id ((char*)item, strlen((char*)item)+1);
        int hashValue = this->sdbm(id) % buckets;
        NodeHT* temp = (NodeHT*)Array[hashValue];
        
        while (temp != NULL) {
            if ( ((Record*)temp->data)->compare_citizenByID(id) ) {    
                return ((Record*)temp->data);
            }
            temp = temp->next;
        }
    }
    else if (!type.compare("SkipList")) {
        string id((char*)item, strlen((char*)item)+1);
        int hashValue = this->sdbm(id) % buckets;
        NodeHT* temp = (NodeHT*)Array[hashValue];
        
        while (temp != NULL) {
            if (!strcmp(((SkipList*)temp->data)->get_virusName(), (char*)item)) {
                return ((SkipList*)temp->data);
            }
            temp = temp->next;
        }
    }
    else if (!type.compare("BloomFilter")) {
        string virusName((char*)item, strlen((char*)item)+1);
        int hashValue = this->sdbm(virusName) % buckets;
        NodeHT* temp = (NodeHT*)Array[hashValue];

        while (temp) {
            if (!strcmp (((BloomFilter*)temp->data)->get_virusName(), (char*)item)) {
                return ((BloomFilter*)temp->data);
            }
            temp = temp->next;
        }
    }
    else if (!type.compare("String")) {
        string str((char*)item, strlen((char*)item)+1);
        int hashValue = this->sdbm(str) % buckets;
        NodeHT* temp = (NodeHT*) Array[hashValue];
        char* temp_item;

        while(temp) {   
            temp_item = (char*)temp->data;
            if (!strcmp(temp_item, (char*)item)) {
                return (char*)temp->data;
            }
            temp = temp->next;
        }
    }

    return NULL;
}


// return the i node 
void* HashTable::get_i_Item(int i)
{
    if (i >= nNodes || i < 0)       //out of hash table
        return NULL;

    NodeHT* temp;

    for (int j = 0; j < buckets; j++)
    {
        temp = (NodeHT*)Array[j];

        while (temp) {
            if (i <= 0)
                return temp->data;
            temp = temp->next;
            i--;
        }
    }

    return NULL;
}



void HashTable::print()
{
    if (!type.compare("Record")) {
        NodeHT* temp;
        for (int i = 0; i < buckets; i++)
        {
            temp = (NodeHT*)Array[i];

            while (temp)
            {
                ((Record*)temp->data)->print_Record();
                temp = temp->next;
            }
        }
    }
    else if (!type.compare("SkipList")) {
        NodeHT* temp;
        for (int i = 0; i < buckets; i++)
        {
            temp = (NodeHT*)Array[i];
            cout << "Bucket: " << i << endl;

            while (temp)
            {
                ((SkipList*)temp->data)->print();
                temp = temp->next;
            }
        }
    }
    else if (!type.compare("BloomFilter"))
    {
        NodeHT* temp;
        for (int i = 0; i < buckets; i++)
        {
            temp = (NodeHT*)Array[i];

            while (temp)
            {
                cout << ((BloomFilter*)temp->data)->get_virusName() << endl;
                temp = temp->next;
            }
        }
    }
    else if (!type.compare("String"))
    {
        NodeHT* temp;
        for (int i = 0; i < buckets; i++)
        {
            temp = (NodeHT*)Array[i];

            while (temp) {
                cout << (char*)temp->data <<endl;
                temp = temp->next;
            }
        }
    }
}


//combine duplicates bloomFilters
void HashTable::update_bloom(BloomFilter* bloom)
{
    NodeHT *node, *temp;
    char* virus;
    for(int i = 0; i < buckets; i++) {
        temp = (NodeHT*)Array[i];

        while(temp) {
            virus = ((BloomFilter*)temp->data)->get_virusName();
            if (!strcmp(virus, bloom->get_virusName())) { 
                ((BloomFilter*)temp->data)->merge_bloom(bloom);
            }
            temp = temp->next;
        }
    }

    
    


}





//hash function
unsigned long HashTable::sdbm(string str_str) {
    unsigned long hash = 0;
    int c;
    char* c_str = strdup(str_str.c_str());
    char* f = c_str;

    while ((c = *c_str++)) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    free (f);
    return hash;
}