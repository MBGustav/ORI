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
    int get_value() const;
    virtual size_t bin_size();
    virtual DataType read_DataType()const ;
    virtual string toString();
    virtual void parseString(string &data);

    virtual void fwrite(std::fstream &file);
    virtual void fread(std::fstream &file);

    // virtual bool compare_val(string data, DataType type) const;
    
    // virtual bool compare_val(DataInterface *left)const;
    virtual bool equal(DataInterface *left)const;
};

IntHandler::IntHandler(int int_num): _data(int_num){}

IntHandler::IntHandler(std::fstream &file){fread(file);}

IntHandler::~IntHandler(){}

size_t IntHandler::bin_size(){return sizeof(int);}

int IntHandler::get_value() const{return _data;}

DataType IntHandler::read_DataType()const {return DataType::INT;}

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

// bool IntHandler::compare_val(DataInterface* left)const{
//     if(!left->read_DataType() != this->read_DataType())
//         throw std::runtime_error("[ERROR] Comparing different types of data");

//     IntHandler *l = dynamic_cast<IntHandler*>(left);
  
//     return this->get_value() >= l->get_value();
// }

bool IntHandler::equal(DataInterface* left)const{
    if(!left->read_DataType() != this->read_DataType())
        throw std::runtime_error("[ERROR] Comparing different types of data");

    IntHandler *l = dynamic_cast<IntHandler*>(left);
  
    return this->get_value() == l->get_value();
}

#endif /*_INTHANDLER_H_*/