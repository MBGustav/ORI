#ifndef _INTHANDLER_H_
#define _INTHANDLER_H_
#include "DataInterface.h"
#include "Date.h"

class IntHandler : public DataInterface
{
private:
    int _data;
public:
    IntHandler(int _data);
    ~IntHandler();
    void set_value(int val);
    int get_value();
    virtual size_t bin_size();
    virtual DataType read_DataType();
    virtual string toString();
    virtual void parseString(string &data);
};

IntHandler::IntHandler(int int_num): _data(int_num){}

IntHandler::~IntHandler(){}

size_t IntHandler::bin_size(){return sizeof(int);}

int IntHandler::get_value(){return _data;}

DataType IntHandler::read_DataType(){return INT;}

void IntHandler::set_value(int val){this->_data = val;}

string IntHandler::toString(){ return std::to_string(get_value());}

void IntHandler::parseString(string &data){set_value(stoi(data));}


#endif /*_INTHANDLER_H_*/