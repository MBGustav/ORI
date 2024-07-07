#ifndef _STRINGHANDLER_H_
#define _STRINGHANDLER_H_

#include "DataInterface.h"
#include <string>
#include <algorithm>
using std::string;




class StringHandler: public DataInterface 
{
private:
    string _data;
public:
    StringHandler(string input);
    ~StringHandler();

    void set_value(string &val);
    virtual size_t bin_size(); 
    virtual DataType read_DataType(); 
    virtual string toString(); 

    virtual void parseString(string &data);
};

StringHandler::StringHandler(string input):_data(input){}

StringHandler::~StringHandler(){}

size_t StringHandler::bin_size(){return std::min(static_cast<size_t>(STR_MAX_SIZE), _data.size());}

DataType StringHandler::read_DataType(){}

string StringHandler::toString(){return string(_data);}

void StringHandler::set_value(string &val){_data = val;}

void StringHandler::parseString(string &data){set_value(data);}

#endif /*_INTHANDLER_H_*/