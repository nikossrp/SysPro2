#ifndef SaveData_H_
#define SaveData_H_
#include "Record.h"
#include "BloomFilter.h"
#include "SkipList.h"   
#include "AdditionTypes.h"
#include "HashTable.h"

class App {     

    private:
        /* Database */
        HashTable* record;
        HashTable* virusNames;
        HashTable* countries;

        /* Bloom Filters */
        HashTable* bloomFilter;
        int bloomSize;

        /*SkipLists*/
        HashTable* vaccinated_list;
        HashTable* no_vaccinated_list;

    public:
        App(int bloomSize, int buckets);
        ~App();
        std::string Insert_Record(char* line);
        char* get_virus(char* virus);
        char* get_country(char* country);
        int get_nCitizens() const { return record->get_nNodes(); }
        int get_nViruses() const { return virusNames->get_nNodes(); }
        int get_nCountries() const { return countries->get_nNodes(); }
        int get_nBloomFilters() const { return bloomFilter->get_nNodes(); }
        int get_nVaccSkipList() const { return vaccinated_list->get_nNodes(); }
        int get_nNoVaccSkipList() const { return no_vaccinated_list->get_nNodes(); }
        HashTable* get_BloomFilters() const { return bloomFilter; }   //return the all bloomfilters
        HashTable* get_vaccinatedList() const { return vaccinated_list; }
        HashTable* get_NoVaccSkipList() const { return no_vaccinated_list; }
        void print_vaccinatedList();
        void print_NoVaccinatedList();
        void print_Records();

        /*********** Erwthmata ************/

        void vaccinatedStatusBloom (char* citizenID, char* virusName);
        char* vaccineStatus (char* citizenID, char* virusName);
        char* vaccineStatus (char* citizenID);
        void populationStatus (char* country, char* virusName, Date* date1, Date* date2);
        void popStatusByAge(char* country, char* virusName, Date* date1, Date* date2);
        std::string insertCitizenRecord (char* s, int dumb = -1);
        std::string vaccinateNow (char* s);
        void list_nonVaccinated_Persons (char* virusName);

        /*********************************/
};



#endif //SaveData_H_