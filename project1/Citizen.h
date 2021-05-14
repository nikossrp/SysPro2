#ifndef Citizen_H_
#define Citizen_H_

#include <iostream>
#include <string>


class Citizen {
    protected:
        std::string citizenID;
        std::string firstName;
        std::string lastName;
        char* country;
        int age;

    public:
        Citizen(std::string citizenID, std::string firstName, std::string lastName, char* country, int age);
        Citizen (const Citizen& cit);
        Citizen() {}
       ~Citizen() {}
        std::string get_id () const { return citizenID; }
        std::string get_firstName () const { return firstName; }
        std::string get_lastName () const { return lastName; }
        char* get_country () const { return country; }
        int get_age () const { return age; }
        void set_id (int id) { this->citizenID = id; }
        void set_firstName (std::string firstName) { this->firstName = firstName; }
        void set_lastName (std::string lastName) { this->lastName = lastName; }
        void set_country (char* country) { this->country = country; }
        void set_age (int age) { this->age = age; }
        bool check_citizenEquality (Citizen* citizen);
        bool compare_citizenByID (std::string citizen);
        void print();
};

#endif //Citizen_H_