#ifndef _DATAINTERFACE_H_
#define _DATAINTERFACE_H_

#include <string>
#include <fstream>
#include <iostream>
using std::string;

#define STR_MAX_SIZE (32)

typedef enum {
    TYPE_NULL,
    DATA_FLOAT,
    DATA_STRING,
    DATA_INT,
    DATA_DATE
} DataType;

// TODO: implement a virtual method that deals with binary -- better to search in tree/index

class DataInterface {
public:
    virtual ~DataInterface() {};

    virtual size_t bin_size() = 0; // Return total size for save/read
    virtual DataType read_DataType() const = 0; // Return type of data
    virtual string toString() = 0;
    virtual void parseString(string &data) = 0;

    // Pointers to simplify read/write to file
    virtual void fwrite(std::fstream &file) = 0;
    virtual void fread(std::fstream &file) = 0;

    bool is_numeric() const {
        return read_DataType() == DataType::DATA_INT || read_DataType() == DataType::DATA_FLOAT;
    };
};

#endif /*_DATAINTERFACE_H_*/