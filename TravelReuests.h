#ifndef Request_H_
#define Request_H_
#include "VaccineMonitor/Date.h"

    
typedef struct info {  // for every country we have a array of dates

    char* virusName;
    char* countryTo;
    Date** date;
    int requests;
    bool* accepted;

    struct info* next;

}TravelInfo;


// this class keep the number of request for every country
class TravelRequests {

    private:
        TravelInfo** travelInfo;
        int buckets;


    public:
        TravelRequests(int buckets);
        ~TravelRequests();
        void InsertRequest(char* virusName, char* countryTo, bool accepted, Date* date);

        void TravelStats(char* virusName, Date* date1, Date* date2, char* countryTo);
        void TravelStats(char* virusName, Date* date1, Date* date2);

        void TravelStats_per_country(char* virusName, Date* date1, Date* date2);
        TravelInfo* get_node(char* virusName, char* countryTo);
        void print();
};


#endif
