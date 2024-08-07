#ifndef _STRINGHANDLER_H_
#define _STRINGHANDLER_H_

#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include "DataInterface.h"


using std::string, std::fstream;

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
    StringHandler(fstream &file);
    ~StringHandler();

    void set_value(string &val);
    string get_value(){return _data;};

    virtual size_t bin_size(); 
    virtual DataType read_DataType();
    virtual string toString(); 

    virtual void parseString(string &data);

    virtual void fwrite(fstream &file);
    virtual void fread(fstream &file);
};

// StringHandler::StringHandler():_data("", STR_MAX_SIZE){}


#endif /*_INTHANDLER_H_*/