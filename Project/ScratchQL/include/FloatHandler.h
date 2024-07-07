#ifndef _FLOATHANDLER_H_
#define _FLOATHANDLER_H_

#include "DataInterface.h"
#include "Date.h"
#include <string>
using std::string;

class FloatHandler : public DataInterface
{
private:
    float _data;
public:
    FloatHandler(float _data);
    ~FloatHandler();
    void set_value(float val);
    float get_value();

    virtual size_t bin_size();
    virtual DataType read_DataType();
    virtual string toString();
    virtual void parseString(string &data);
};

FloatHandler::FloatHandler(float float_num): _data(float_num){}

FloatHandler::~FloatHandler(){}

float FloatHandler::get_value(){return _data;}

size_t FloatHandler::bin_size(){return sizeof(float);}

DataType FloatHandler::read_DataType(){return FLOAT;}

void FloatHandler::set_value(float val){this->_data = val;}

string FloatHandler::toString(){ return std::to_string(get_value());}

void FloatHandler::parseString(string &data){set_value(std::stof(data));}


#endif /*_DATAHANDLER_H_*/