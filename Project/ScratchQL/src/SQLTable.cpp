
#include <cstring>
#include "ErrorHandler.h"
#include "SQLTable.h"
#include "MetaData.h"


string format_print(data_t val2print)
{   
    DataType tp = val2print.tp;
    switch(tp)
    {
        case STRING:  return val2print.value_str;
        case INT   :  return to_string(val2print.value_int);
        case FLOAT :  return to_string(val2print.value_float);
        case BOOL  :  return (val2print.value_bool ? "True" : "False");
        default: Type_error();
    }
    return "";
}




/*Criacao de Uma Tabela(sobrescrita caso tenha o mesmo nome?)*/
SQLTable::SQLTable(const string TableName,
                   vector<EntityProp>Entity_values,
                   bool is_temp = false):
        _temporary_table(is_temp), _metadata(TableName, Entity_values){}


SQLTable::SQLTable(const string TableName) :
            _metadata(TableName)
{
}


void SQLTable::Delete()
{
    if(is_temporary())
        _metadata.Delete();
}

SQLTable::~SQLTable()
{
    this->Delete();
}



void SQLTable::showTable()
{
    auto prevFormat = std::cout.flags();
    size_t totalEntities = _metadata.total_entities();
    size_t offset = _metadata.get_sizeHeader();
    size_t Rows = _metadata.get_numRows();
    int max = 15;

    //Tabularizacao 

    auto pretty_line = [&](vector<data_t> Line_Print){
        for(auto &it: Line_Print)
            std::cout <<"| "<< setw(max) << left << format_print(it) ;
        std::cout << " |" << std::endl;
    };

    auto pretty_entities = [&](vector<EntityProp> &Line_Print){        
        for(auto &it: Line_Print)
            std::cout <<"| "<< setw(max) << left << it.Name ;
        std::cout << " |" << std::endl;
        
    };
    
    std::cout << "TABLE NAME :" << this->getName() <<std::endl;
    vector<EntityProp> ent = _metadata.get_entities();
    
    pretty_entities(ent);
    
    // for(int row = 0; row < min(Rows, (size_t) TOTAL_DATA_DISPLAY); row++)
    // {
    //     vector<data_t> rowData = this->readRow(row);
    //     pretty_line(rowData);

    // }

    std::cout << "Rows Affected: " << _metadata.get_numRows() <<endl;
    std::cout.flags(prevFormat);
}




bool  SQLTable::insertRow(vector<data_t>Data)
{
    return false;    
}
vector<data_t> SQLTable::readRow(size_t idx)
{
    return vector<data_t>();
}
