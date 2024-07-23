#ifndef _FLOATHANDLER_H_
#define _FLOATHANDLER_H_

#include <string>
#include <stdexcept>
#include <fstream>
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
    float get_value();

    virtual size_t bin_size();
    virtual DataType read_DataType();
    virtual string toString();
    virtual void parseString(string &data);

    virtual void fwrite(std::fstream &file);
    virtual void fread(std::fstream &file);    
};

#endif /*_DATAHANDLER_H_*/