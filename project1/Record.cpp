#include <iostream>
#include "Record.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

Record::Record(string citizenID, std::string firstName, std::string lastName, char* country, int age, Info* info) : Citizen(citizenID, firstName, lastName, country, age)
{
    this->nVirusNames = 1;  //arxika eisagoume ena virus
    this->viruses_info = (Info**)malloc(sizeof(Info*));
    this->viruses_info[0] = info;
}


Record::~Record()
{
    for (int i = 0; i < nVirusNames; i++) {
        if ( this->viruses_info[i]->vaccinated )
            delete viruses_info[i]->date;
        free(viruses_info[i]);
    }

    free(viruses_info);
}


void Record::Insert_virus(Info* info)         //desmevse xwro gia to inf apo tin arxhs sto sygekrimeno record!!
{
    nVirusNames++;
    Info** more_virus = (Info**)realloc(viruses_info, nVirusNames*sizeof(Info*));

   if(more_virus != NULL) {
       viruses_info = more_virus;
       viruses_info[nVirusNames-1] = info;   //tha exei desmevtei apo to application o ios stin mnimi
   }
   else {
       puts("Error allocating memory in Insert_Info");
       free(viruses_info);
       exit(1);
   }
}


void Record::updateInfo(Info* info)
{
    for (int i = 0; i < this->get_nVirus(); i++)
    {
        if( !strcmp(viruses_info[i]->virusName, info->virusName)  ) {
            
            free(viruses_info[i]);    //delete the old info   (NO vaccin)
            viruses_info[i] = info;     //update the info for the virus
            return;     //to chekarisma gia to an einai vacc h oxi ginete sto app
        }
    }       
}



Record::Record(const Record& rec): Citizen(rec)
{

    for(int i = 0; i < nVirusNames; i++) 
    {
        viruses_info[i]->vaccinated = rec.viruses_info[i]->vaccinated;
        viruses_info[i]->virusName = rec.viruses_info[i]->virusName;
        viruses_info[i]->date = rec.viruses_info[i]->date;
    }
}



bool Record::find_virus(char* virus)
{
    for (int i = 0; i < nVirusNames; i++)
    {
        if( !strcmp(viruses_info[i]->virusName, virus)  )
            return true;
    }

    return false;
}


/*An brei ton io ston pinaka tou record tote
epistrefei tin hmerominia, alliws NULL*/
Date* Record::check_vaccinated(char* virus)
{
    for (int i = 0; i < nVirusNames; i++)
    {
        if( !strcmp(viruses_info[i]->virusName, virus)  )
            if (viruses_info[i]->vaccinated)
                return viruses_info[i]->date;
    }
    return NULL;
}

void Record::print_citizen()
{
    this->print();      //print citizen 
}


void Record::print_Record()
{
    for (int i = 0; i < nVirusNames; i++)
    {
        cout << citizenID << " " << firstName << " "
            << lastName << " " << country << " " << age << " ";
        cout << viruses_info[i]->virusName;
        cout << (viruses_info[i]->vaccinated ? " Yes ": " NO ");

        if(viruses_info[i]->vaccinated)
            cout << *(viruses_info[i]->date);
        cout << endl;
    }
}


void Record::print_info_for_virus(char* virus)
{
    if (this->find_virus(virus))
    {
        for (int i = 0; i < nVirusNames; i++)
        {
            if (!strcmp(viruses_info[i]->virusName, virus)) {
                cout << viruses_info[i]->virusName <<
                    ((viruses_info[i]->vaccinated) ? " YES "  : " NO");
                if ( viruses_info[i]->vaccinated )
                    cout << *viruses_info[i]->date;
                cout << endl;
            }
        }
    }
    else
        cout << " VIRUS NOT FOUNT " << endl;
}