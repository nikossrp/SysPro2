#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "TravelReuests.h"

using namespace std;

TravelRequests::TravelRequests(int buckets)
{
    this->buckets = buckets;
    this->travelInfo = (TravelInfo**) malloc(buckets * sizeof(TravelInfo*));

    for (int i = 0; i < buckets; i++) {
        travelInfo[i] = NULL;
    }
}


//hash function
unsigned long sdbm(string str_str) {
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



TravelRequests::~TravelRequests()
{
    TravelInfo *temp, *fr;

    for (int i = 0; i < buckets; i++)
    {
        temp = travelInfo[i];
        fr = temp;

        while (temp)
        {   
            temp = temp->next;
            
            for(int i = 0; i < fr->requests; i++) {
                delete fr->date[i];
            }
            free(fr->date);
            free(fr->virusName);
            free(fr->countryTo);
            free(fr->accepted);
            free(fr);
            
            fr = temp;
        }

    }
    free(travelInfo);
}




// hash string : virusName + countryTo
void TravelRequests::InsertRequest(char* c_virusName, char* countryTo, bool accepted, Date* date)
{
    int d = date->get_day();
    int m = date->get_month();
    int y = date->get_year();
    
    char* country = strdup(countryTo);
    char* virusName = strdup(c_virusName);


    Date* alloc_date = new Date(d, m, y);
    TravelInfo* check = this->get_node(virusName, country);
    if (check != NULL) {
        int i = ++check->requests;
        check->date = (Date**) realloc(check->date, i * sizeof(Date*));
        check->date[i-1] = alloc_date;
        check->accepted = (bool*)realloc(check->accepted, i * sizeof(bool));
        check->accepted[i-1] = accepted;

        free(country);
        free(virusName);

        return;
    }
    else {

        char* str = (char*) malloc(100 * sizeof(char));
        strcpy(str, virusName);
        strcat(str, country);

        int hashValue = sdbm(str) % buckets;
        free(str);


        TravelInfo* node = (TravelInfo*) malloc(sizeof(TravelInfo));
        node->countryTo = country;
        node->virusName = virusName;
        node->requests = 0;
        int i = ++node->requests;
        node->date = NULL;
        node->date = (Date**) realloc(node->date, i*sizeof(Date*));
        node->date[i-1] = alloc_date;
        node->accepted = NULL;
        node->accepted = (bool*)realloc(node->accepted, i * sizeof(bool));
        node->accepted[i-1] = accepted;
        node->next = NULL;

        TravelInfo* temp = (TravelInfo*) travelInfo[hashValue];

        while(temp && temp->next != NULL) {
            temp = temp->next;
        }

        if (temp) {
            temp->next = node;
        }
        else 
            travelInfo[hashValue] = node;
    }
}


// get node need a country and a virus 
// to find the information about requests
TravelInfo* TravelRequests::get_node(char* virusName, char* countryTo)
{
    char* str = (char*) malloc(100 * sizeof(char));
    strcpy(str, virusName);
    strcat(str, countryTo);
    int hashValue = sdbm(str) % buckets;
    free(str);

    TravelInfo* temp = (TravelInfo*)travelInfo[hashValue];
    while(temp != NULL) {
        if (!strcmp(temp->virusName, virusName) &&
            !strcmp(temp->countryTo, countryTo)) {
            return temp;
        }
        temp = temp->next;
    }

    return NULL;
}


void TravelRequests::TravelStats(char* virus, Date* date1, Date* date2, char* country)
{
    char* virusName = strdup(virus);
    char* countryTo = strdup(country);

    TravelInfo* temp = this->get_node(virusName, countryTo);
    int total_requests = temp->requests;
    int accepted = 0, rejected = 0;


    for(int i = 0; i < temp->requests; i++)
    {
        if (*temp->date[i] >= *date1 && *temp->date[i] <= *date2) {
            accepted += (temp->accepted[i] ? 1 : 0);
            rejected += (temp->accepted[i] ? 0 : 1);
        }
    }

    // We need total request for date1 - date2
    cout << "TOTAL REQUESTS " << accepted + rejected << endl;
    cout << "ACCEPTED " << accepted << endl;
    cout << "REJECTED " << rejected << endl;

    free(virusName);
    free(countryTo);

}


// print statistics for every country between date1, date2
void TravelRequests::TravelStats_per_country(char* virus, Date* date1, Date* date2)
{
    char* virusName = strdup(virus);

    TravelInfo* temp = NULL;
    int total_requests = 0, accepted = 0, rejected = 0;
    for(int i = 0; i < buckets; i++)
    {
        temp = travelInfo[i];
        total_requests = accepted = 0;

        while(temp) {

            if (!strcmp(temp->virusName, virusName)) {
                total_requests = temp->requests;

                for(int j = 0; j < total_requests; j++) {
                    if (*temp->date[j] >= *date1 && *temp->date[j] <= *date2) {
                        accepted += (temp->accepted[j] ? 1 : 0);
                        rejected += (temp->accepted[j] ? 0 : 1);
                    }
                }

                cout << temp->countryTo << endl;
                cout << "TOTAL REQUESTS " << accepted + rejected << endl;
                cout << "ACCEPTED " << accepted << endl;
                cout << "REJECTED " << rejected << endl << endl;
            }

            temp = temp->next;
        }
    }

    free(virusName);
}


// print for all countries together between date1, date2
void TravelRequests::TravelStats(char* virus, Date* date1, Date* date2)
{
    char* virusName = strdup(virus);

    TravelInfo* temp = NULL;
    int total_requests = 0, accepted = 0, rejected = 0;
    for(int i = 0; i < buckets; i++)
    {
        temp = travelInfo[i];

        while(temp) {

            if (!strcmp(temp->virusName, virusName)) {
                total_requests = temp->requests;

                for(int j = 0; j < total_requests; j++) {
                    if (*temp->date[j] >= *date1 && *temp->date[j] <= *date2) {
                        accepted += (temp->accepted[j] ? 1 : 0);
                        rejected += (temp->accepted[j] ? 0 : 1);
                    }
                }

            }

            temp = temp->next;
        }
    }

    cout << "TOTAL REQUESTS " << accepted + rejected << endl;
    cout << "ACCEPTED " << accepted << endl;
    cout << "REJECTED " << rejected << endl << endl;

    free(virusName);
}



void TravelRequests::print()
{

    TravelInfo* temp;
    for (int i = 0; i < buckets; i++)
    {
        temp = travelInfo[i];

        while (temp)
        {
            cout << "VirusName: " << temp->virusName << " Country: " << temp->countryTo << " Requests: " << temp->requests << endl;
            cout << "Dates:\n";
            for(int j = 0; j < temp->requests; j++) {
                cout << j << ": " << *temp->date[j] << (temp->accepted[j] ? " ACCEPTED" : " REJECTED") << endl;
            }
            temp = temp->next;
            cout << endl;
        }
    }

}


