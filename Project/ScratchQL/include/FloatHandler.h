#ifndef _FLOATHANDLER_H_
#define _FLOATHANDLER_H_

#include <string>
#include "DataInterface.h"

// #include "Date.h"
using std::string;

class FloatHandler : public DataInterface {
private:
    float _data;
public:

    FloatHandler(float _data = 0.0f);
    ~FloatHandler();
    void set_value(float val);
    float get_value() const;
    

    virtual size_t bin_size();
    virtual DataType read_DataType() const;
    virtual string toString();
    virtual void parseString(string &data);

    virtual void fwrite(std::fstream &file);
    virtual void fread(std::fstream &file);

    // virtual bool compare_val(DataInterface *left)const;
    // virtual bool compare_val(string data, DataType type) const;

    virtual bool equal(DataInterface *left)const;
};

FloatHandler::FloatHandler(float float_num): _data(float_num){}

FloatHandler::~FloatHandler(){}

float FloatHandler::get_value() const{return _data;}

size_t FloatHandler::bin_size(){return sizeof(float);}

DataType FloatHandler::read_DataType() const { return DataType::FLOAT;}

void FloatHandler::set_value(float val){this->_data = val;}

string FloatHandler::toString(){ return std::to_string(get_value());}

void FloatHandler::parseString(string &data){set_value(std::stof(data));}

void FloatHandler::fwrite(std::fstream &file){
    file.write(reinterpret_cast<char*>(this), sizeof(*this));
}

void FloatHandler::fread(std::fstream &file) {
    file.read(reinterpret_cast<char*>(this), sizeof(*this));
}

// bool FloatHandler::compare_val(DataInterface* left)const{
//     if(!left->read_DataType() != this->read_DataType())
//         throw std::runtime_error("[ERROR] Comparing different types of data");

//     FloatHandler *l = reinterpret_cast<FloatHandler*>(&left);
  
//     return this->get_value() >= l->get_value();
// }


bool FloatHandler::equal(DataInterface* left)const{
    if(!left->read_DataType() != this->read_DataType())
        throw std::runtime_error("[ERROR] Comparing different types of data");

    FloatHandler *l = dynamic_cast<FloatHandler*>(left);
  
    return this->get_value() == l->get_value();
}


#endif /*_DATAHANDLER_H_*/