#ifndef _DATAHANDLER_H_
#define _DATAHANDLER_H_

#include "DateHandler.h"


DateHandler::DateHandler(string &input)
{
    //strip down string to write DATE

    int day, month, year;
    
    day   = stoi(input.substr(0,2));
    month = stoi(input.substr(3,5));
    year  = stoi(input.substr(6,10));

    NewDate(day, month, year);
}
DateHandler::DateHandler(int month, int day, int year)
{
    NewDate(day, month, year);
}
DateHandler::DateHandler(){}

DataType DateHandler::read_DataType(){return DataType::DATE;}

DateHandler::DateHandler(Date &date): _date(date)
{

}

void DateHandler::NewDate(int day, int month, int year)
{
    if(is_valid(day, month, year)) _date = Date(day,month, year);
    else _date = Date(1, 1, 1);
}


DateHandler::~DateHandler(){}

size_t DateHandler::bin_size(){return _date.size();}

string DateHandler::toString(){return _date.toString();}

void DateHandler::parseString(string &data){
    //check if is valid before;
    _date.parseString(data);
}


void DateHandler::fwrite(std::fstream &file){
    file.write(reinterpret_cast<const char*>(&_date), sizeof(_date));
}

void DateHandler::fread(std::fstream &file){
    file.read(reinterpret_cast<char*>(&_date), sizeof(_date));
}

#endif /*_DATAHANDLER_H_*/