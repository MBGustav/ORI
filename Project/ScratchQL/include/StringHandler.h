#ifndef _STRINGHANDLER_H_
#define _STRINGHANDLER_H_

#include <string>
#include <cstring>
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
    StringHandler(std::fstream &file);
    ~StringHandler();

    void set_value(string &val);
    string get_value() const{return _data;};

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

// StringHandler::StringHandler():_data("", STR_MAX_SIZE){}

StringHandler::StringHandler(string input):_data(input){}

StringHandler::StringHandler(std::fstream &file){fread(file);};

StringHandler::~StringHandler(){}

size_t StringHandler::bin_size(){return static_cast<size_t>(STR_MAX_SIZE);}

DataType StringHandler::read_DataType()const  { return DataType::STRING;}

string StringHandler::toString()
{
    if(!_data.data()) std::cout << "NULL access\n";
    return _data;
}

void StringHandler::set_value(string &val){_data = val;}

void StringHandler::parseString(string &data){set_value(data);}

void StringHandler::fwrite(std::fstream &file){
    
    char tmp[STR_MAX_SIZE] = {0};
    char sz = (char) _data.size();
    tmp[STR_MAX_SIZE-1] = sz; //fill size (cast to char)
    memcpy(tmp, _data.data(), std::min((int) _data.size(),(int) sz));
    file.write(tmp, STR_MAX_SIZE);

}
void StringHandler::fread(std::fstream &file){
    if (!file.is_open()) {
        throw std::runtime_error("File not open");
    }

    using namespace std;
    char tmp[STR_MAX_SIZE-1];
    file.read(tmp, STR_MAX_SIZE);

    char sz = static_cast<char>(tmp[STR_MAX_SIZE - 1]);
    _data.assign(tmp, sz); // Only assign up to the actual string size
}

// bool StringHandler::compare_val(DataInterface* left)const{
//     if(!left->read_DataType() != this->read_DataType())
//         throw std::runtime_error("[ERROR] Comparin different types of data");

//     StringHandler *l = dynamic_cast<StringHandler*>(left);
  
//     return this->get_value() >= l->get_value();
// }
    

bool StringHandler::equal(DataInterface* left)const{
    if(!left->read_DataType() != this->read_DataType())
        throw std::runtime_error("[ERROR] Comparin different types of data");

    StringHandler *l = dynamic_cast<StringHandler*>(left);
  
    return this->get_value() == l->get_value();
}

#endif /*_INTHANDLER_H_*/