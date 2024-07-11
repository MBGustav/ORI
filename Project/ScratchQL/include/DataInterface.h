#ifndef _DATAINTERFACE_H_
#define _DATAINTERFACE_H_

#include <string>
using std::string;

#define STR_MAX_SIZE (35)

typedef enum{
    TYPE_NULL,
    FLOAT, 
    STRING,
    INT,
    DATE
}DataType;

class DataInterface
{
    private:
    public:
    ~DataInterface(){};

    virtual size_t bin_size() = 0; //return total size for save/read
    virtual DataType read_DataType() = 0; // return type of data
    virtual string toString() = 0; 
    virtual void parseString(string &data) = 0;

};

#endif /*_DATAINTERFACE_H_*/