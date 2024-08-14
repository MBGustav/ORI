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

#endif /*_DATAHANDLER_H_*/