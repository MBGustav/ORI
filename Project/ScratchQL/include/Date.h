#ifndef _DATE_H_
#define _DATE_H_

#include <sstream>
#include <iomanip>
#include <string>
const std::string Month[12] = { "Janeiro", "Fevereiro", "Março", "Abril",
                                "Maio", "Junho","Julho", "Agosto", 
                                "Setembro", "Outubro", "Novembro", "Dezembro"};
const int day_month[12] ={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

using std::string;

class Date
{
private:

    int d, m, y;
public:
    Date();
    Date(int day, int month, int year);
    ~Date();
    int get_day()   const;
    int get_month() const;
    int get_year()  const;

    void set_date(int month, int day, int year);
    
    bool is_valid(int _m, int _d, int _y);
    bool is_bissext(int _y);
    size_t size() const;  
    string toString() const;
    void parseString(string &data);
    Date operator=(const Date& date);
};


#endif /*_DATE_H_*/