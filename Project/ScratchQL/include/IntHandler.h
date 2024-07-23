#ifndef _INTHANDLER_H_
#define _INTHANDLER_H_

#include "DataInterface.h"
#include <stdexcept>
#include <fstream>

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

#endif /*_INTHANDLER_H_*/