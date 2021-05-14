#ifndef Date_H_
#define Date_H_

#include <iostream>
#include <chrono>
#include <ctime>


class Date {
    private:
        int day;
        int month;
        int year;

    public:
        Date(int day, int month, int year);
        Date() {}
        Date(const Date& date);
        ~Date() {};
        void set_day(int d) { day = d; }
        void set_month(int m) { month = m; }
        void set_year(int y) { year = y; }
        int get_day() const { return day; }
        int get_month() const { return month; }
        int get_year() const { return year; }
        int accepted_date6Months(Date* date);
        char* get_date_charStar();
        bool operator==(const Date& dat);
        Date& operator=(const Date& dat);
        bool operator<=(const Date& dat);
        bool operator<(const Date& dat);
        bool operator>(const Date& dat);
        bool operator>=(const Date& dat);
        void print();
        friend std::ostream &operator<<(std::ostream& left, const Date& ob);

};

#endif // Date_H_
