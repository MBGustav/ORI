#ifndef _DATAHANDLER_H_
#define _DATAHANDLER_H_

#include "DateHandler.h"
#include <iostream>



DateHandler::DateHandler(string &input)
{
    //strip down string to write DATE

    int day, month, year;
    
    day   = stoi(input.substr(0,2));
    month = stoi(input.substr(3,5));
    year  = stoi(input.substr(6,10));

    NewDate(day, month, year);

    bool is_valid(int m, int d, int y);


}

void DateHandler::NewDate(int month, int day, int year)
{
    m = month;
    d = day;
    y = year;
}
DateHandler::DateHandler(int month, int day, int year)
{
    NewDate(day, month, year);
}
DateHandler::DateHandler(){}

DataType DateHandler::read_DataType(){return DataType::DATE;}

DateHandler::DateHandler(DateHandler &date) 
{
    y = date.get_year();
    m = date.get_month();
    d = date.get_day();
}

DateHandler::~DateHandler(){}


string DateHandler::toString(){
    string ans(std::to_string(d) + "/" +
               std::to_string(m) + "/" +
               std::to_string(y));
    return ans;
}

void DateHandler::parseString(string &data){
    //check if is valid before;
    // _date.parseString(data);
}

void DateHandler::fwrite(std::fstream &file){
    file.write(reinterpret_cast<const char*>(this), sizeof(*this));
}

void DateHandler::fread(std::fstream &file){
   if(file.read(reinterpret_cast<char*>(this), sizeof(*this)))
        std::cout << "OK\n";
    else std::cout << "not OK\n";
}

bool DateHandler::is_bissext(int _ano){
    return (_ano % 400 == 0) || ( (_ano % 4 == 0) && (_ano % 100 != 0) );
}

bool DateHandler::is_valid(int _mes, int _dia, int _ano)
{
    int feb_add = 0;
    if(is_bissext(_ano) && _mes == 2) feb_add = 1;
    bool date_OK = (0 <= _ano) && (0 < _mes && _mes <=12) && (0<= _dia && _dia <= day_month[_mes -1]+feb_add);
    return date_OK;
}


size_t DateHandler::bin_size() {return sizeof(*this);};  


#endif /*_DATAHANDLER_H_*/