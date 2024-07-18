#ifndef _DATEHANDLER_H_
#define _DATEHANDLER_H_

#include "DataInterface.h"
// #include "Date.h"
#include <string>
#include <fstream>

using std::string;


const std::string Month[12] = { "Janeiro", "Fevereiro", "Março", "Abril",
                                "Maio", "Junho","Julho", "Agosto", 
                                "Setembro", "Outubro", "Novembro", "Dezembro"};
const int day_month[12] ={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


class DateHandler: public DataInterface
{
private:
    // Date _date;
    int d,m,y;
public:
    DateHandler();
    DateHandler(DateHandler &date);
    DateHandler(string &input);
    DateHandler(int month, int day, int year);
    ~DateHandler();

    void NewDate(int month, int day, int year);
    bool is_valid(int day, int month, int year);
    void set_date(int month, int day, int year);


    int get_day()   const{return this->d;};
    int get_month() const{return this->m;};
    int get_year()  const{return this->y;};
    bool is_bissext(int _y);

    virtual size_t bin_size();
    virtual DataType read_DataType();
    virtual string toString();
    virtual void parseString(string &data);

    virtual void fwrite(std::fstream &file);
    virtual void fread(std::fstream &file);
};


#endif /*_DATEHANDLER_H_*/