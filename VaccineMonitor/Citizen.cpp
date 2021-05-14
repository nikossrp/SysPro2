#include "Citizen.h"
#include <iostream>
#include <string>
#include <string.h>

using namespace std;

Citizen::Citizen(string citizenID, string firstName, string lastName, char* country, int age)
{
    // while(1);
    this->citizenID = citizenID;
    this->firstName = firstName;
    this->lastName = lastName;
    this->country = country;
    this->age = age;
}

Citizen::Citizen (const Citizen& cit)
{
    citizenID = cit.citizenID;
    firstName = cit.firstName;
    lastName = cit.lastName;
    country = cit.country;
    age = cit.age;
}

bool Citizen::compare_citizenByID(string id)
{   
    if (!strcmp(citizenID.c_str(), id.c_str()))     
        return true;
    return false;
}

bool Citizen::check_citizenEquality(Citizen* citizen)
{   
    return (!citizenID.compare(citizen->get_id())) && !(firstName.compare(citizen->get_firstName()))
        && !(lastName.compare(citizen->get_lastName())) && !(strcmp(country, citizen->get_country())) 
        && (age == citizen->get_age());
}

void Citizen::print()
{
    cout << citizenID << " " << firstName << " " << lastName << " "
     << country << " " <<  age;
}

