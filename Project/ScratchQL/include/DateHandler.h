#ifndef _DATEHANDLER_H_
#define _DATEHANDLER_H_

#include "DataInterface.h"
#include "Date.h"
#include <string>
#include <fstream>
using std::string;


class DateHandler: public DataInterface
{
private:
    Date _date;
public:
    DateHandler();
    DateHandler(Date &date);
    DateHandler(string &input);
    DateHandler(int month, int day, int year);
    ~DateHandler();

    void NewDate(int month, int day, int year);

    bool is_valid(int day, int month, int year);
    
    virtual size_t bin_size();
    virtual DataType read_DataType();
    virtual string toString();
    virtual void parseString(string &data);

    virtual void fwrite(std::fstream &file);
    virtual void fread(std::fstream &file);
};


#endif /*_DATEHANDLER_H_*/