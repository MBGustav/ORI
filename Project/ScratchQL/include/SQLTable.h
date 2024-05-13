#ifndef _SQLTable_H_
#define _SQLTable_H_ 

#include <iostream> 
#include <string> 
#include <vector>
#include <iomanip>
#include <bitset>


#include "MetaData.h"
#include "ErrorHandler.h"

using namespace std;


class SQLTable{
private:

    MetaData _metadata;
    bool _temporary_table; // controle de tabelas temporarias

    
    void Delete();
    void setName(string FileName){_metadata.set_name(FileName);};

public: 
    
    // SQLTable();
    //This Case we create a Table or update it
    SQLTable(const string TableName,vector<EntityProp>Entity_values, bool is_temp);

    //In case we have a table, open it
    SQLTable(const string TableName);

    ~SQLTable();
    
    //getters
    string getName() const {return _metadata.get_name(false);};
    bool is_temporary()const{ return this->_temporary_table;};
    
    //operations
    void showTable();

    
    bool insertRow(vector<data_t>Data);
    bool deleteRow(vector<data_t>Data);// TODO: Conceito de Primary key
    vector<data_t> readRow(size_t idx);

};


string format_print(data_t val2print);

#endif /*_SQLTable_H_*/