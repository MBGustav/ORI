#ifndef _DATAINTERFACE_H_
#define _DATAINTERFACE_H_

#include <string>
using std::string;
// #include "IntHandler.h"
// #include "FloatHandler.h"
// #include "DateHandler.h"
// #include "StringHandler.h"

#define STR_MAX_SIZE (32)

typedef enum{
    TYPE_NULL,
    FLOAT, 
    STRING,
    INT,
    DATE
}DataType;


// TODO : implement a virtual method that deals with binary --  better to search in three/ index

class DataInterface
{
    public:
    virtual ~DataInterface(){};

    virtual size_t bin_size() = 0; //return total size for save/read
    virtual DataType read_DataType()const = 0; // return type of data
    virtual string toString() = 0;
    virtual void parseString(string &data) = 0;

    //Pointers to simplify read/write to file
    virtual void fwrite(std::fstream &file) = 0;
    virtual void fread(std::fstream &file)  = 0;
     
    // constraint: being used with same base class
    // virtual bool compare_val(DataInterface* left)const =0;
    // virtual bool compare_val(string data, DataType type) const = 0;
    virtual bool equal(DataInterface* left)const =0;

    bool is_numeric() const{return read_DataType() == DataType::INT || read_DataType() == DataType::FLOAT;};
};




#endif /*_DATAINTERFACE_H_*/