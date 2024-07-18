#ifndef _INTHANDLER_H_
#define _INTHANDLER_H_

#include "DataInterface.h"


class IntHandler : public DataInterface
{
private:
    int _data;
public:
    IntHandler(int _data = 0);
    IntHandler(std::fstream &file);

    ~IntHandler();
    void set_value(int val);
    int get_value();
    virtual size_t bin_size();
    virtual DataType read_DataType();
    virtual string toString();
    virtual void parseString(string &data);

    virtual void fwrite(std::fstream &file);
    virtual void fread(std::fstream &file);
};

IntHandler::IntHandler(int int_num): _data(int_num){}

IntHandler::IntHandler(std::fstream &file){fread(file);}

IntHandler::~IntHandler(){}

size_t IntHandler::bin_size(){return sizeof(int);}

int IntHandler::get_value(){return _data;}

DataType IntHandler::read_DataType(){return DataType::INT;}

void IntHandler::set_value(int val){this->_data = val;}

string IntHandler::toString(){ return std::to_string(get_value());}

void IntHandler::parseString(string &data){set_value(stoi(data));}


void IntHandler::fwrite(std::fstream &file) {
    if (!file.is_open()) {
        throw std::runtime_error("File not open");
    }
    file.write(reinterpret_cast<char*>(&_data), sizeof(_data));
}

void IntHandler::fread(std::fstream &file) {
    if (!file.is_open()) {
        throw std::runtime_error("File not open");
    }
    file.read(reinterpret_cast<char*>(&_data), sizeof(_data));
}

#endif /*_INTHANDLER_H_*/