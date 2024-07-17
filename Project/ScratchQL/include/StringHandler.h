#ifndef _STRINGHANDLER_H_
#define _STRINGHANDLER_H_

#include <string>
#include <algorithm>

#include "DataInterface.h"
using std::string;


#ifndef STR_MAX_SIZE
#define STR_MAX_SIZE (35)
#endif


class StringHandler: public DataInterface 
{
private:
    // char str_data[STR_MAX_SIZE];
    string _data;
public:
    StringHandler() = default;
    StringHandler(string input);
    ~StringHandler();

    void set_value(string &val);
    virtual size_t bin_size(); 
    virtual DataType read_DataType();
    virtual string toString(); 

    virtual void parseString(string &data);

    virtual void write_to_file(std::fstream &file);
    virtual void read_from_file(std::fstream &file);
};

// StringHandler::StringHandler():_data("", STR_MAX_SIZE){}

StringHandler::StringHandler(string input):_data(input){}

StringHandler::~StringHandler(){}

size_t StringHandler::bin_size(){return std::min(static_cast<size_t>(STR_MAX_SIZE), _data.size());}

DataType StringHandler::read_DataType() { return DATE;}

string StringHandler::toString(){return string(_data);}

void StringHandler::set_value(string &val){_data = val;}

void StringHandler::parseString(string &data){set_value(data);}

void StringHandler::write_to_file(std::fstream &file){
    
    char tmp[STR_MAX_SIZE-1] = {0};
    unsigned char sz = (unsigned char) bin_size();
    tmp[0] = sz; //fill size (cast to unsigned char)
    
    file.write(tmp, STR_MAX_SIZE);

}
void StringHandler::read_from_file(std::fstream &file){
    char tmp[STR_MAX_SIZE];
    unsigned char sz;
    file.read(tmp, STR_MAX_SIZE);
    
    sz = (unsigned char) tmp[0]; //fill size (cast to unsigned char)
    
    _data.resize(sz);
    memcpy(_data.data(), tmp+1, sz);
}

#endif /*_INTHANDLER_H_*/