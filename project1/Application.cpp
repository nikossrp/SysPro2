#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <ctime>
#include "Application.h"

using namespace std;

App::App(int bloomSize, int buckets)
{
    buckets = (buckets ? buckets : MAXSIZE);
    int lessBuckets = ((buckets/4) ? buckets/4 : buckets);   //for virusNames 

    this->record = new HashTable (buckets, "Record");

    this->virusNames = new HashTable (lessBuckets, "String");
    this->countries = new HashTable (NUMBER_OF_COUNTRIES/10, "String");
    this->bloomFilter = new HashTable (lessBuckets, "BloomFilter");
    this->bloomSize = bloomSize;
    this->vaccinated_list = new HashTable (lessBuckets, "SkipList");
    this->no_vaccinated_list = new HashTable (lessBuckets, "SkipList");
}


App::~App()
{
    delete virusNames;
    delete countries;
    delete record;
    delete bloomFilter;
    delete vaccinated_list;
    delete no_vaccinated_list;
}


/* Elenxoi:
*   An uparxei NO + date
*   An to  age < 0 || age >= 120, h an exei gramma
*   An to record uparxei hdh (Eite id + diaforetika ta alla 4 pedia, eite to idio record me ton idio io)
*   An to date einai consistent ws pros tin morfh (2 pavles,date < date_now, 0<d<31 - 0<m<13),
*   [ An to date einai mikrotero h iso apo thn shmerini hmerominia ]
*   An to firstname, lastName, country einai consistent (dhladh xwris arithmous)
*   An to id exei mono arithmous
*/
string App::Insert_Record(char* s)
{
    int nWords = countWords(s);
    char* rec = strdup(s);
    int flag_incons = 0;
    Info* info = (Info*) malloc(sizeof(Info));       //all info about (no) vaccinated

    if (nWords > 8 || nWords < 7) {     //inconsistent records
        cout << "ERROR IN RECORD " << s << endl;
        free (info);
        free (s);
        free(rec);
        return "-1";
    }

    /* ID */
    char* c_id = strtok(s, " ");
    string id(c_id, strlen(c_id));

    if (!consistentNumber(c_id)) {
        cout << "ERROR IN RECORD " << rec << endl;
        free (info);
        free (s);
        free(rec);
        return "-1";
    }

    /* Name */
    char* c_name = strtok(NULL, " ");
    string firstname_str(c_name, strlen(c_name));


    /* Last Name */
    char* c_lastName = strtok(NULL, " ");
    string lastName_str(c_lastName, strlen(c_lastName));

    /* Country */
    char* c_country = strtok(NULL, " ");
    string country_str(c_country, strlen(c_country));
    char* country;

    if (!consistentName(c_name) || !consistentName(c_lastName) || !consistentName(c_country)) {
        cout << "ERROR IN RECORD " << rec << endl;
        free (info);
        free(s);
        free(rec);
        return "-1";
    }

    /* Age */
    char* c_age = strtok(NULL, " ");
    int age;
    sscanf(c_age, "%d", &age);

    if (!consistentNumber(c_age) || (age > 120 || age <= 0)) {
        cout << "ERROR IN RECORD " << rec << endl;
        free (info);
        free(s);
        free(rec);
        return "-1";
    }

    if (this->get_country(c_country) == NULL)     //An den uparxei h xwra stin lista prosthese thn
    {
        country = (char*)countries->Insert((char*)c_country) ;
    }
    else {
        country = this->get_country(c_country);
    }


    /* Virus Name */
    char* c_virusName = strtok(NULL, " ");
    char* virusName;

    if (this->get_virus(c_virusName) == NULL)  //An den uparxei o ios ston pinaka prosthese ton
    {
        virusName = (char*)virusNames->Insert((char*)c_virusName);

        BloomFilter* bloom = new BloomFilter(virusName, bloomSize);
        bloomFilter->Insert((BloomFilter*)bloom);

        SkipList* VskipList = new SkipList(virusName);
        vaccinated_list->Insert((SkipList*)VskipList);

        SkipList* NVskipList = new SkipList(virusName);
        no_vaccinated_list->Insert((SkipList*)NVskipList);

    }
    else {
        virusName = this->get_virus(c_virusName);
    }

    
    /* Vaccinated */
    char* c_vaccinated = strtok(NULL, " ");
    bool vaccin = false;

    if ( (!strcmp(c_vaccinated, "YES") || !strcmp(c_vaccinated, "NO")) ||
        (!strcmp(c_vaccinated, "Yes") || !strcmp (c_vaccinated, "No")) ) {

        if ( !(strcmp(c_vaccinated, "YES")) || !(strcmp(c_vaccinated, "Yes")) )
        {
            vaccin = true;
        }
        else if (!(strcmp(c_vaccinated, "NO")) || !(strcmp(c_vaccinated, "No")) )
        {
            vaccin = false;
        }
    }
    else {
        cout << "ERROR IN RECORD " << rec << endl;
        free(info);
        free(rec);
        free(s);
        return "-1"; 
    }

    /* Date */
    Date* date = NULL;

    // inconsistence records
    if( (!vaccin && nWords > 7 ) || (vaccin && nWords > 7) ) {   
        date = new Date;

        char* c_date = strtok(NULL, " ");

        if (!consistentDate_str(c_date)) {  //checkare to format tou date
            cout << "ERROR IN RECORD " << id << " "<< firstname_str 
                << " " << lastName_str << " " << country_str << " " << age 
                << " "<< c_virusName << " YES " << c_date << endl;

            delete date;
            free(info);
            free(rec);
            free(s);
            return "-1";      //error insert record
        }

        /* Day */
        char* c_day = strtok(c_date, "-");
        int day;
        sscanf(c_day, "%d", &day);
        date->set_day(day);

        /* Month */
        char* c_month = strtok(NULL, "-");
        int month;
        sscanf(c_month, "%d", &month);
        date->set_month(month);

        /* Year */
        char* c_year = strtok(NULL, "-");
        int year;
        sscanf(c_year, "%d", &year);
        date->set_year(year);
        
    }
    else date = NULL;

    
    info->date = date;
    info->vaccinated = vaccin;
    info->virusName = virusName;     

   
    Citizen* citizen = new Citizen(id, firstname_str, lastName_str, country, age);


    //bres ton citizen me to idio id 
    Record* temp = (Record*)record->get_item((char*)c_id);  

    if (temp) {   
        if (temp->find_virus(c_virusName)) {    //bres an uparxei o idios ios sthn eggrafh
            flag_incons = 1;     
        }
    }

    //check inconsistency  id same but not all others
    if ( temp && (!temp->get_id().compare(id)) && !(temp->check_citizenEquality(citizen)) ) {
        flag_incons = 1;
    }


    if ( (!vaccin && (nWords != 7)) || flag_incons ||
        !consistentDate(date) || (vaccin && nWords != 8) ) { 
        
        if(vaccin && nWords == 8) {
            cout << "ERROR IN RECORD " << id << " "<< firstname_str << " "
                << lastName_str << " " << country_str  << " "
                << age << " " << c_virusName 
                << " YES " << *date << endl;

            delete date;
        }
        else if (!vaccin && (nWords > 7) ) {
            cout << "ERROR IN RECORD " << id << " "<< firstname_str << " "
                << lastName_str << " " << country_str  << " "
                << age << " " << c_virusName 
                << " NO " << *date << endl;
            delete date;
        }
        else if( !vaccin ) {
            cout << "ERROR IN RECORD " << id << " "<< firstname_str 
                << " " << lastName_str << " " << country_str << " " << age 
                << " "<< c_virusName << " NO" << endl;
        }
        else if (vaccin && nWords < 8) {
            cout << "ERROR IN RECORD " << id << " "<< firstname_str 
                << " " << lastName_str << " " << country_str << " " << age 
                << " "<< c_virusName << " YES" << endl;
        }

        delete citizen;
        free(info);
        free(rec);
        free(s);
        return "-1";      
    }



    // efoson exei perasei ola ta tests gia consistency mporei na eisaxthei
    /* Se periptwsh pou uparxei to record mesa sto 
        database prosthetoume to neo io ston pinaka tou
        kai se oles tis domes (BloomFilter - SkipLists) */
    SkipList* temp_sl;

    if (temp)
    {
        temp->Insert_virus(info);       //insert virus for citizen

        if (vaccin) {
            BloomFilter* temp_bl = (BloomFilter*)bloomFilter->get_item(c_virusName);
            temp_bl->Insert_Record(c_id);
            temp_sl = (SkipList*)vaccinated_list->get_item(c_virusName);
            temp_sl->Insert(temp);
        }
        else if (!vaccin) {
            temp_sl = (SkipList*)no_vaccinated_list->get_item(c_virusName);
            temp_sl->Insert(temp);
        }

        delete citizen;
        free(rec);
        free(s);
        return id;
    }


    Record* new_record = new Record(id, firstname_str, lastName_str, country, age, info);

    if (vaccin) { 
        BloomFilter* temp_bl = (BloomFilter*)bloomFilter->get_item(c_virusName);
        temp_bl->Insert_Record(c_id);
        temp_sl = (SkipList*)vaccinated_list->get_item(c_virusName);
        temp_sl->Insert(new_record);
    }
    else if (!vaccin) {
        temp_sl = (SkipList*)no_vaccinated_list->get_item(c_virusName);
        temp_sl->Insert(new_record);

    }

    record->Insert(new_record);
    delete citizen;
    free(s);
    free(rec);


    return id;
}



char* App::get_virus(char* virus)
{
    char* p = (char*)virusNames->get_item((char*)virus);
    return p;
}



char* App::get_country(char* country)
{
    char* p = (char*)countries->get_item((char*)country);
    return p;
}


void App::print_Records()
{
    record->print();
}


void App::print_vaccinatedList()
{
    for (int i = 0; i < virusNames->get_nNodes(); i++) {
        char* virusName = (char*)virusNames->get_i_Item(i);
        ((SkipList*)vaccinated_list->get_item((char*)virusName))->print();
    }
}

void App::print_NoVaccinatedList()
{
    for (int i = 0; i < virusNames->get_nNodes(); i++) {
        // cout <<"\n******" << virusNames[i] << "******"<< endl;
        char* virusName = (char*)virusNames->get_i_Item(i);
        ((SkipList*)no_vaccinated_list->get_item((char*)virusName))->print();
    }
}






/****************************************** Erwthmata ******************************************/

void App::vaccinatedStatusBloom(char* citizenID, char* virusName)
{
    Record* rec = (Record*)record->get_item(citizenID);

    if (!rec) {
        cout << "Record with citizen id " << citizenID << " didn't founded\n";
        return;
    }

    BloomFilter* temp = (BloomFilter*)bloomFilter->get_item((char*)virusName);
    if (temp)
    {
        if (!temp->check_Record(citizenID))
        {
            cout << "NOT VACCINATED" << endl;
            return;
        }
        else {
            cout << "MAYBE" << endl;
            return;
        }
    }
    cout << "Virus with name " << virusName << " didn't founded\n";
}


//An den uparxei sto vaccinate-list ektupwnei NO
char* App::vaccineStatus(char* citizenID, char* virusName)
{
    string id_str(citizenID, strlen(citizenID) + 1);
    char* virus = strdup(virusName);
    SkipList* temp = (SkipList*)vaccinated_list->get_item((char*)virus);
    Record* rec = (Record*)record->get_item(citizenID);
    char* results = (char*) malloc(100 * sizeof(char));

    if (!rec) {
        // cout << "Record with citizen id " << citizenID << " didn't founded\n";
        strcat(results, "None");
        free(virus);
        return results;
    }

    if (temp) 
    {
        Record* record = temp->get_record(id_str);
        
        if (record) {
            Date* date = record->check_vaccinated(virus);
            // cout << "VACCINATED ON " << *date << endl; 
            strcpy(results, "YES ");
            char d[10] = "";
            char m[10] = "";
            char y[10] = "";

            sprintf(d, "%d", date->get_day());
            sprintf(m, "%d", date->get_month());
            sprintf(y, "%d", date->get_year());

            strcat(results, d);
            strcat(results, "-");
            strcat(results, m);
            strcat(results, "-");
            strcat(results, y);

            free(virus);
            return results;
        }
        else {
            // cout << "NOT VACCINATED" << endl;
            strcpy(results, "NO");
            free(virus);
            return results;
        }
    }

    // cout << "Virus with name " << virusName << " didn't founded\n";
    free(virus);
    strcat(results, "None");

    return results;
}


//Ektupwnei NO an uparxei sto no_vaccinate_list
char* App::vaccineStatus(char* citizenID)
{
    Record* record1, *record2;
    string id_str (citizenID, strlen(citizenID)+1);
    Record* rec = (Record*)record->get_item(citizenID);
    char* results = (char*) malloc(2000 * sizeof(char));
    memset(results, '\0', 2000);

    if (!rec) {
        // cout << "Record with citizen id " << citizenID << " didn't founded\n";
        strcpy(results, "None");
        return results;
    }
    

    strcat(results, rec->get_id().c_str());
    strcat(results, " ");
    strcat(results, rec->get_firstName().c_str());
    strcat(results, " ");
    strcat(results, rec->get_lastName().c_str());
    strcat(results, " ");
    strcat(results, rec->get_country());
    strcat(results, "\n");

    char* age = (char*) malloc(sizeof(char) * 100);
    memset(age, '\0', 100);

    sprintf(age, "%d", rec->get_age());
    strcat(results, "Age ");
    strcat(results, age);
    strcat(results, "\n");

    free(age);


    for (int i = 0; i < virusNames->get_nNodes(); i++)
    {
        //take the skip list from hashtable
        char* virusName = (char*)virusNames->get_i_Item(i);
        SkipList* temp_VS = (SkipList*)vaccinated_list->get_item((char*)virusName);
        SkipList* temp_NVS = (SkipList*)no_vaccinated_list->get_item((char*)virusName);

        record1 = temp_VS->get_record(id_str);  //take record from vaccinate skip list 
        record2 = temp_NVS->get_record(id_str); //take record from no vaccinate skip list
        if (record1) {
            strcat(results, virusName);
            strcat(results, " VACCINATED ON ");
            Date vacc_date = *(record1->check_vaccinated(virusName));
            strcat(results, vacc_date.get_date_charStar());
            strcat(results, "\n");
        }
        else if (record2) {
            strcat(results, virusName);
            strcat(results, " NOT YET VACCINATED");
            strcat(results, "\n");
        }
    }

    return results;
}



void App::populationStatus (char* country, char* virusName, Date* date1, Date* date2)
{
    int count = 0;
    int num_NoVacc = 0;
    int total_num_Yes = 0;
    int nCountries = countries->get_nNodes();
    int* noVacc_ByCountry = new int[nCountries];
    char* virus = strdup(virusName);

    for (int i = 0; i < countries->get_nNodes(); i++)
    {
        noVacc_ByCountry[i] = 0;
    }

    /*take the number of no vaccinated in country*/
    SkipList* temp_NVS = (SkipList*)no_vaccinated_list->get_item((char*)virus); 
    if (temp_NVS)
    {
        if (country) {
            num_NoVacc = temp_NVS->countStatusByCountry_NoVacc(country);
        }
        else {
            for (int j = 0; j < nCountries; j++) {
                country = (char*)countries->get_i_Item(j);
                noVacc_ByCountry[j] = temp_NVS->countStatusByCountry_NoVacc(country);
            }
            country = NULL;
        }
    }

    SkipList* temp_VS = (SkipList*)vaccinated_list->get_item((char*)virus);

    if (temp_VS) 
    {
        if (country) {
            count = temp_VS->countStatusByCountry(country, date1, date2);
            total_num_Yes = temp_VS->countStatusByCountry(country);

            if ((num_NoVacc == 0) && (total_num_Yes == 0))  //avoiding   1 / 0
                num_NoVacc = 1;

            cout << endl;
            printf("%s  %d  %.2f %%\n", country ,count, ((float)count / (num_NoVacc + total_num_Yes)) * 100.0);
        }
        else if (!country) {
            for (int j = 0; j < countries->get_nNodes(); j++) {
                country = (char*)countries->get_i_Item(j);
                count = temp_VS->countStatusByCountry(country, date1, date2);
                total_num_Yes = temp_VS->countStatusByCountry(country);

                if ((noVacc_ByCountry[j] == 0) && (total_num_Yes == 0)) 
                    noVacc_ByCountry[j] = 1;

                cout << endl;
                printf("%s  %d  %.2f %%\n", country, count, (float)count/(noVacc_ByCountry[j] + total_num_Yes) * 100.0);
            }
        }

    }
    else {
        cout << "Virus with name " << virusName << " didn't founded\n";

    }

    delete [] noVacc_ByCountry;
    free (virus);
}





void App::popStatusByAge(char* country, char* virusName, Date* date1, Date* date2)
{
    if (!date1 || !date2) {
        cout << "ERROR\n";
        return;
    }

    int nCountries = countries->get_nNodes();
    int** noVacc_ByCountry = new int*[nCountries+1];  //otan den exei dwthei country
    int* no_VaccByAge;      //otan exei dwthei country
    int* total_num_yes;
    char* temp_country;                   
    int* Ages = NULL;
    
    char* virus = strdup(virusName);

    /*Take the number of no vaccinated in country of the correct ages*/
    SkipList* temp_NVS = (SkipList*)no_vaccinated_list->get_item((char*)virus);

    if (temp_NVS) {
        if (country) {
            no_VaccByAge = temp_NVS->countStatusBy_Age_Country_NoVacc(country);
        }
        else if (!country) {
            for (int j = 0; j < countries->get_nNodes(); j++) {
                country = (char*)countries->get_i_Item(j);
                noVacc_ByCountry[j] = temp_NVS->countStatusBy_Age_Country_NoVacc(country);
            }
            country = NULL;
        }
    }
    else {
        cout << "Virus with name " << virusName << " didn't founded\n";
        delete [] noVacc_ByCountry;
        free (virus);
        return;
    }

    SkipList* temp_VS = (SkipList*) vaccinated_list->get_item((char*)virus);

    //for vaccinated persons
    if (temp_VS) {
        if (country) {
            Ages = temp_VS->countStatusBy_Age_Country(country, virus, date1, date2);
            total_num_yes = temp_VS->countStatusBy_Age_Country(country);

            for (int j = 0; j < 4; j++) {       // 4 einai ta diasthmata tis hlikias
                if (no_VaccByAge[j] == 0 && total_num_yes[j] == 0)
                    no_VaccByAge[j] = 1;
            }

            cout << endl;
            cout << country << endl;
            printf("0-20 \t%d  %.2f %%\n", Ages[0], (float)Ages[0] /( total_num_yes[0] + no_VaccByAge[0]) * 100.0 );
            printf("20-40\t%d  %.2f %%\n", Ages[1], (float)Ages[1] /( total_num_yes[1] + no_VaccByAge[1]) * 100.0 );
            printf("40-60\t%d  %.2f %%\n", Ages[2], (float)Ages[2] /( total_num_yes[2] + no_VaccByAge[2]) * 100.0 );
            printf("60+ \t%d  %.2f %%\n", Ages[3], (float)Ages[3] /( total_num_yes[3] + no_VaccByAge[3]) * 100.0 );

            delete [] Ages;
            delete [] noVacc_ByCountry;
            delete [] no_VaccByAge;
            delete [] total_num_yes;
            free(virus);
            return;
        }
        else if (!country) {
            for (int j = 0; j < countries->get_nNodes(); j++) {
                temp_country = (char*)countries->get_i_Item(j);
                Ages = temp_VS->countStatusBy_Age_Country(temp_country, virusName , date1, date2);
                for (int i = 0; i < 4; i++) {
                    if ((noVacc_ByCountry[j][i] == 0) && (Ages[i] == 0))
                        noVacc_ByCountry[j][i] = 1;
                }
                
                cout << endl;
                cout << temp_country << endl;
                printf("0-20 \t%d  %.2f %%\n", Ages[0], (float)Ages[0] /( Ages[0] + noVacc_ByCountry[j][0]) * 100.0);
                printf("20-40 \t%d  %.2f %%\n", Ages[1], (float)Ages[1] /( Ages[1] + noVacc_ByCountry[j][1]) * 100.0);
                printf("40-60 \t%d  %.2f %%\n", Ages[2], (float)Ages[2] /( Ages[2] + noVacc_ByCountry[j][2]) * 100.0);
                printf("60+ \t%d  %.2f %%\n\n", Ages[3], (float)Ages[3] /( Ages[3] + noVacc_ByCountry[j][3]) * 100.0);
                delete [] Ages;
            }
        }
    }


    for (int i = 0; i < countries->get_nNodes(); i++)
        delete [] noVacc_ByCountry[i];
    
    delete [] noVacc_ByCountry;

    free (virus);
}

/*
*   Gia dumb = 1 exei tin leitourgeia tis vaccinateNow

*   Genika einai paromoia me tin Insert_Record
    me allages stous elenxous kai sta error messages
*/
string App::insertCitizenRecord (char* s, int dumb)
{
    char* rec = strdup(s);
    int nWords = countWords(s);
    int flag_incons = 0;
    int flag_noVacci = 0;
    int flag_newVirus = 0;
    Info* info = (Info*) malloc(sizeof(Info));       //all info about (not)vaccination  

    if (dumb != 1 && (nWords > 8 || nWords < 7)) {     //inconsistent records
        cout << "ERROR IN RECORD " << s << endl;
        free (info);
        free (s);
        free(rec);
        return "-1";
    }

    /* ID */
    char* c_id = strtok(s, " ");
    string id(c_id, strlen(c_id));

    if (!consistentNumber (c_id)) {
        cout << "ERROR IN RECORD: citizenID must has only numbers" << rec << endl;
        free (info);
        free (s);
        free(rec);
        return "-1";
    }

    /* Name */
    char* c_name = strtok(NULL, " ");
    string firstname_str(c_name, strlen(c_name));

    /* Last Name */
    char* c_lastName = strtok(NULL, " ");
    string lastName_str(c_lastName, strlen(c_lastName));

    /* Country */
    char* c_country = strtok(NULL, " ");
    string country_str(c_country, strlen(c_country));
    char* country;

    if (!consistentName(c_name) || !consistentName(c_lastName) || !consistentName(c_country)) {
        cout << "ERROR IN RECORD " << rec << endl;
        free (info);
        free(rec);
        free(s);
        return "-1";
    }

    /* Age */
    char* c_age = strtok(NULL, " ");

    if (!consistentNumber(c_age)) {
        cout << "ERROR IN RECORD " << rec << endl;
        free (info);
        free(rec);
        free(s);
        return "-1";
    }

    if (this->get_country(c_country) == NULL)     //An den uparxei h xwra stin lista prosthese thn
    {
        country = (char*)countries->Insert((char*)c_country);   //return a pointer to name
    }
    else
        country = this->get_country(c_country);


    int age;
    sscanf(c_age, "%d", &age);

    if (!consistentNumber (c_age) || (age > 120 || age <= 0)) {
        cout << "ERROR IN RECORD " << rec << endl;
        free (info);
        free(s);
        free(rec);
        return "-1";
    }


    /* Virus Name */
    char* c_virusName = strtok(NULL, " ");

    char* virusName;

    if(this->get_virus(c_virusName) == NULL)  //An den uparxei o ios stin lista prosthese ton
    {
        virusName = (char*)virusNames->Insert((char*)c_virusName);
        BloomFilter* bloom = new BloomFilter(virusName, bloomSize);
        bloomFilter->Insert((BloomFilter*)bloom);

        SkipList* VskipList = new SkipList(virusName);
        vaccinated_list->Insert((SkipList*)VskipList);

        SkipList* NVskipList = new SkipList(virusName);
        no_vaccinated_list->Insert((SkipList*)NVskipList);
    }
    else {
        virusName = this->get_virus(c_virusName);
    }
    
    bool vaccin = false;
    if (dumb != 1) {

        /* Vaccinated */
        char* c_vaccinated = strtok(NULL, " ");
        if ( (!strcmp(c_vaccinated, "YES") || !strcmp(c_vaccinated, "NO")) ||
            (!strcmp(c_vaccinated, "Yes") || !strcmp (c_vaccinated, "No")) ) {

            if ( !(strcmp(c_vaccinated, "YES")) || !(strcmp(c_vaccinated, "Yes")) )
            {
                vaccin = true;
            }
            else if (!(strcmp(c_vaccinated, "NO")) || !(strcmp(c_vaccinated, "No")) )
            {
                vaccin = false;
            }
        }
        else {
            cout << "ERROR IN RECORD " << rec << endl;
            free(info);
            free(rec);
            free(s);
            return "-1"; 

        }
    }
    else 
        vaccin = true;


    /* Date */
    Date* date = NULL;

    if (dumb == 1 && vaccin) {
        time_t t = time(0);   
        struct tm * now = localtime( & t ); // get time now
        int day = now->tm_mday;
        int month = now->tm_mon + 1;
        int year = now->tm_year + 1900;
        date = new Date(day, month, year);
    }
    else {

        if ( (!vaccin && nWords > 7 ) || (vaccin && nWords > 7) ) {   
            date = new Date;

            char* c_date = strtok(NULL, " ");
                
            if (!consistentDate_str(c_date)) {  //checkare to format tou date ws string
                cout << "ERROR IN RECORD " << id << " "<< firstname_str 
                    << " " << lastName_str << " " << country_str << " " << age 
                    << " "<< c_virusName << " YES " << c_date << endl;

                delete date;
                free(info);
                free(rec);
                free(s);
                return "-1";      
            }

            /* Day */
            char* c_day = strtok(c_date, "-");
            int day;
            sscanf(c_day, "%d", &day);
            date->set_day(day);

            /* Month */
            char* c_month = strtok(NULL, "-");
            int month;
            sscanf(c_month, "%d", &month);
            date->set_month(month);

            /* Year */
            char* c_year = strtok(NULL, "-");
            int year;
            sscanf(c_year, "%d", &year);
            date->set_year(year);
        }
        else date = NULL;
    }
    
    info->date = date;
    info->vaccinated = vaccin;
    info->virusName = virusName;     

    Citizen* citizen = new Citizen(id, firstname_str, lastName_str, country, age);
    Date* temp_date;

    //find record + date if there is
    Record* old_Record = (Record*) record->get_item(c_id); 

    //check inconsistency  id same but not all others
    if ( old_Record && (!old_Record->get_id().compare(id)) &&
        !(old_Record->check_citizenEquality(citizen)) ) {
        flag_incons = 1;
    }

    
    if (dumb == 1) {    
        if ( flag_incons ) { //inconsistent vaccinate now record

            cout << "ERROR IN RECORD " << id << " "<< firstname_str 
                << " " << lastName_str << " " << country_str << " " << age 
                << " "<< c_virusName << " YES " << *date << endl;

            delete citizen;
            delete date;
            free(info);
            free(s);
            free(rec);
            return "-1";      //error insert record
        }
    }
    else if ( (!vaccin && (nWords != 7)) || (age > 120 || age <= 0) || flag_incons ||
            !consistentDate(date) || (vaccin && nWords != 8) ) { 
        
        if(vaccin && (nWords == 8)) {
            cout << "ERROR IN RECORD " << id << " "<< firstname_str << " "
                << lastName_str << " " << c_country  << " "
                << age << " " << c_virusName 
                << " YES " << *date << endl;

            delete date;
        }
        else if (!vaccin && (nWords > 7)) {
            cout << "ERROR IN RECORD " << id << " "<< firstname_str << " "
                << lastName_str << " " << c_country  << " "
                << age << " " << c_virusName 
                << " NO " << *date << endl;

            delete date;
        }
        else if( !vaccin ) {
            cout << "ERROR IN RECORD " << id << " "<< firstname_str 
                << " " << lastName_str << " " << c_country << " " << age 
                << " "<< c_virusName << " NO" << endl;
        }
        else if (vaccin && nWords < 8) {
            cout << "ERROR IN RECORD " << id << " "<< firstname_str 
                << " " << lastName_str << " " << c_country << " " << age 
                << " "<< c_virusName << " YES" << endl;
        }

        delete citizen;
        free(info);
        free(rec);
        free(s);
        return "-1";      
    }

    if (old_Record && old_Record->find_virus(c_virusName)) {

        temp_date = old_Record->check_vaccinated(c_virusName);  //an uparxei hmerominia emvoliasmoy

        if (temp_date) {
            cout << "ERROR: CITIZEN " << id << " ALREADY VACCINATED ON " <<  *temp_date << endl;
            if (date)
                delete date;
            delete citizen;
            free(info);
            free(s);
            free(rec);
            return "-1";    
        }
        else if (!temp_date && !vaccin) {   //htan No kai to xanakanei No (invalid(?))
            cout << "ERROR IN RECORD " << id << " "<< firstname_str 
                << " " << lastName_str << " " << country_str << " " << age 
                << " "<< c_virusName << " NO" << endl;
                    delete citizen;
            free(info);
            free(s);
            free(rec);
            return "-1";     
        }
        else if (!temp_date && vaccin) {  // htan NO kai thelei na to kanei YES
            flag_noVacci = 1;
        }
    }
    else if (old_Record && !old_Record->find_virus(c_virusName)) {
        flag_newVirus = 1;  //exoume new virus gia ton citizen
    }

    SkipList* temp_sl;

    if (old_Record && flag_newVirus)
    {
    /* Se periptwsh pou uparxei to record mesa sto database
        prosthetoume to neo io ston pinaka tou
        kai se oles tis domes (BloomFilter - SkipLists) */
        old_Record->Insert_virus(info);
        if (vaccin) {       
            BloomFilter* temp_bl = (BloomFilter*)bloomFilter->get_item(c_virusName);
            temp_bl->Insert_Record(c_id);
            temp_sl = (SkipList*)vaccinated_list->get_item(c_virusName);
            temp_sl->Insert(old_Record);
        }
        else if (!vaccin) {
            temp_sl = (SkipList*)no_vaccinated_list->get_item(c_virusName);
            temp_sl->Insert(old_Record);
        }

        delete citizen;
        free(s);
        free(rec);
        return id;
    }

    /*Se periptwsh pou apo NO ginei yes*/
    if (old_Record && flag_noVacci) 
    {
        old_Record->updateInfo(info);     //update information about virus in record
        ((SkipList*)no_vaccinated_list->get_item((char*)c_virusName))->Delete(old_Record); //delete from no_vaccinated_list
        ((BloomFilter*)bloomFilter->get_item(c_virusName))->Insert_Record(c_id);    //insert in BloomFilter
        ((SkipList*) vaccinated_list->get_item(c_virusName))->Insert(old_Record);  //insert in vaccinated list

        delete citizen;
        free(s);
        free(rec);
        return id;
    }

    Record* new_record = new Record(id, firstname_str, lastName_str, country, age, info);

    if (vaccin) { 
        BloomFilter* temp_bl = (BloomFilter*)bloomFilter->get_item(c_virusName);
        temp_bl->Insert_Record(c_id);
        temp_sl = (SkipList*)vaccinated_list->get_item(c_virusName);
        temp_sl->Insert(new_record);
    }
    else if (!vaccin) {
        temp_sl = (SkipList*)no_vaccinated_list->get_item(c_virusName);
        temp_sl->Insert(new_record);

    }

    record->Insert(new_record);
    delete citizen;
    free(s);
    free(rec);

    return id;
}


string App::vaccinateNow(char* s)
{
    return this->insertCitizenRecord(s, 1);
}


void App::list_nonVaccinated_Persons(char* virusName)
{   
    SkipList* temp_NVS = (SkipList*)no_vaccinated_list->get_item((char*)virusName);
    
    if (temp_NVS) {
       if (!temp_NVS->print_citizens()) 
        cout << "No records for the virus " << virusName << endl;
    }
    else{
        cout << "Virus with name " << virusName << " didn't founded\n";
    }
}







