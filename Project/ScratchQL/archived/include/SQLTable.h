#ifndef _SQLTable_H_
#define _SQLTable_H_ 

#include <iostream> 
#include <string> 
#include <vector>
#include <iomanip>
#include <bitset>
#include <map>


#include "MetaData.h"
#include "ErrorHandler.h"

// using namespace std;


// TODO: quais parametros devo inserir no map?
typedef struct map_data{ 
    size_t index;
    size_t other_param;/*TODO: remover*/
}map_data;
/*
*/ 



class SQLTable{
private:

    bool _temporary_table;               // controle de tabelas temporarias
    MetaData _metadata;
    /*
    TODO: O Usuario deve definir no inicio da concepcao de tabela a sua chave
    Preciso de um marcador no metadado que especifique quem seria a pkey.
    int-> especifica o indice da entidade.
    BD: chave primaria como primeiro campo, facilitando a ordem na criacao.
    */
    std::map<int, map_data> pkey_map;    // controle de pkeys

    
    void setName(string FileName){_metadata.set_name(FileName);};
    
    //Obtem chaves da Pkey[TODO: alem das pkeys, oque devo armazenar? posicao no]
    void upload_pkeys();

public: 
    
    // SQLTable();
    //This Case we create a Table or update it
    SQLTable(const string TableName,vector<EntityProp>Entity_values, bool is_temp);

    //In case we have a table, open it
    SQLTable(const string TableName);

    ~SQLTable();
    void Delete();
    
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