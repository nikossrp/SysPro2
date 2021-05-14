#ifndef Record_H_
#define Record_H_

#include <iostream>
#include <string>
#include "Citizen.h"
#include "Date.h"
#include "AdditionTypes.h"

/* To kathe record ektos apo ta stoixeia tou citizen
*  exei kai ena array me ta virus
*/

class Record : public Citizen {
        private:
            Info** viruses_info;
            int nVirusNames;

        public:
            Record(std::string citizenID, std::string firstName, std::string lastName, char* country, int age, Info* info);
            Record(const Record& rec);
            ~Record();
            void Insert_virus (Info* info);
            void updateInfo(Info* info);   
            int get_nVirus () const { return nVirusNames; }
            bool find_virus(char* virus);
            Date* check_vaccinated(char* virus);
            void print_citizen();
            void print_Record();
            void print_info_for_virus(char* virus);
};



#endif //Record_H_