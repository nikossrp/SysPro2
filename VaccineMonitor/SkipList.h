#ifndef SkipList_H_
#define SkipList_H_

#include <iostream>
#include "Nodes.h"

#define MaxHeight 10   

class SkipList {
    private:
        Node* sentinel;
        char* virusName;
        int numNodes;

    public:
        SkipList (char* virusName);
        ~SkipList ();
        void Insert (Record* rec);
        void Delete (Record* c_id);
        Record* get_record (std::string id_str);
        int get_NumNodes () const { return numNodes; }
        int get_height () const { return sentinel->height; }
        char* get_virusName () const { return virusName; }
        int countStatusByCountry(char* country, Date* date1, Date* date2);
        int countStatusByCountry (char* country);
        int* countStatusBy_Age_Country (char* country, char* virusName, Date* date1, Date* date2);  
        int* countStatusBy_Age_Country(char* country);      //gia tous total_num_yes
        int* countStatusBy_Age_Country_NoVacc (char* country);
        int countStatusByCountry_NoVacc(char* country);

        void print();
        int print_citizens();

};


#endif // SkipList_H_