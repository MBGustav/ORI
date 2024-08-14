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

#endif /*_INTHANDLER_H_*/