#include <iostream>
#include "Date.h"
#include <string.h>

using namespace std;
using namespace std::chrono;

Date::Date(int day, int month, int year)
{
    this->day = day;
    this->month = month;
    this->year = year;
}

Date::Date(const Date& date)
{
    day = date.day;
    month = date.month;
    year = date.year;
}

Date& Date::operator= (const Date& date)
{
    if (this == &date) return *this;
    day = date.day;
    month = date.month;
    year = date.year;
    return *this;
}


bool Date::operator==(const Date& d)
{
    return (day == d.day) && (month == d.month) && (year == d.year); 
}

//output format: 01-02-2016
ostream &operator<<(ostream& left, const Date& ob)
{
    int day_i = ob.get_day();
    string day_str = to_string(day_i);
    int month_i = ob.get_month();
    string month_str = to_string(month_i);
    int year_i = ob.get_year();
    string year_str = to_string(year_i);

    left << ((day_str.length()==1) ? "0" + day_str : day_str ) << "-" 
        << ((month_str.length()==1) ? "0" + month_str : month_str ) << "-" 
        << ((year_str.length()==1) ? "0" + year_str : year_str );
        
    return left;
}

//checks if date is less than 6 months before the desired date
int Date::accepted_date6Months(Date* travel_date)
{
    if (*this > *travel_date) {
        //you need vaccination
        return 0; 
    }
    else if (*this == *travel_date) {
        return 1;
    }
    else if (*this < *travel_date) {

        int d_lim = travel_date->get_day();
        int m_lim = travel_date->get_month();
        int y_lim = travel_date->get_year();

        m_lim -= 6;
        if (m_lim < 0) {
            m_lim = 12 + m_lim;
            y_lim -= 1;
        }

        Date* date_lim = new Date(d_lim, m_lim, y_lim);

        if (*this > *date_lim) {
            delete date_lim;
            return 1;
        }
        else {
            delete date_lim;
            return 0;
        }
    }

    return 0;
}


char* Date::get_date_charStar()
{
    char* date = (char*) malloc(100 * sizeof(char));
    char* day = (char*) malloc(sizeof(char) * 5);
    char* month = (char*) malloc(sizeof(char) * 5);
    char* year = (char*) malloc(sizeof(char) * 10);
    memset(date, '\0', 100);
    memset(day, '\0', 5);
    memset(month, '\0', 5);
    memset(year, '\0', 10);


    sprintf(day, "%d", this->get_day());
    sprintf(month, "%d", this->get_month());
    sprintf(year, "%d", this->get_year());

    strcat(date, day);
    strcat(date, "-");
    strcat(date, month);
    strcat(date, "-");
    strcat(date, year);
    
    free(day);
    free(month);
    free(year);

    return date;
}


bool Date::operator<(const Date& d)
{
    if (year < d.get_year()) {

        return true;
    }
    else if (year > d.get_year()) {
        return false;
    }
    else {  //same year
        if (month < d.get_month()) {
            return true;
        }
        else if (month > d.get_month()) {
            return false;
        }
        else { //same month
            if (day < d.get_day())
                return true;
            else 
                return false;
        }
        
    }

}


bool Date::operator>(const Date& dat)
{
    if (*this == dat)
        return false;

    if (*this < dat)
        return false;

    return true;
}


bool Date::operator>=(const Date& d)
{
    if (*this == d)
        return true;
    
    if (*this > d)
        return true;

    return false;

}


bool Date::operator<=(const Date& d)
{
    if (*this == d)
        return true;

    if (*this < d)
        return true;
        
    return false;
}



void Date::print()
{
    cout << day << "-" << month << "-" << year;
}
