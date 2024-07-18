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


class DataInterface
{
    public:
    virtual ~DataInterface(){};

    virtual size_t bin_size() = 0; //return total size for save/read
    virtual DataType read_DataType() = 0; // return type of data
    virtual string toString() = 0; 
    virtual void parseString(string &data) = 0;

    //Pointers to simplify read/write to file
    virtual void fwrite(std::fstream &file) = 0;
    virtual void fread(std::fstream &file)  = 0;

};




#endif /*_DATAINTERFACE_H_*/