#ifndef _METADATA_H_
#define _METADATA_H_ 

#include <iostream> 
#include <string> 
#include <vector>
#include <fstream>
#include <algorithm>
#include "ErrorHandler.h"

#define ENTITY_SIZE_NAME (32)
#define STR_SIZE (ENTITY_SIZE_NAME)
#define MAX_ENTITIES (10) 
#define MAX_READ_SIZE (MAX_ENTITIES * ENTITY_SIZE_NAME) 
#define TOTAL_DATA_DISPLAY (12)


#define TABLE_EXT ".tab"
#define DIRECTORY_FOR_TABLES "FolderTables/"


using namespace std;



typedef enum DataType{
    BOOL,
    INT,
    FLOAT, 
    STRING
}DataType;

typedef struct EntityProp{
    // char Name[ENTITY_SIZE_NAME];
    string Name;
    DataType type;
}EntityProp;

typedef struct data_t 
{
    DataType tp;
    union{
        char  value_str[STR_SIZE];
        int   value_int;
        float value_float;
        bool  value_bool;
    };

}data_t;

typedef vector<EntityProp> v_entity;

/*TODO: implementar uma forma mais eficiente de coletar os dados em arquivo*/
/*atualmente, é definido o tamanho por meio do maior campo -- union*/
int inline size_of(DataType value)
{
    switch (value){
        case BOOL:   return sizeof(bool);
        case INT:    return sizeof(int);
        case FLOAT:  return sizeof(float);
        case STRING:   return sizeof(char) * ENTITY_SIZE_NAME;
        
        default: exit(-1);
    }

    return -1;
}
/*
Classe para Lidar com as principais informações de uma tabela (sem fazer a sua leitura total)
    - total_idx:      quantidade de linhas de dados
    - offset_row:     distancia(fixa entre cada indice)
    - total_entities: total de entidades presentes na tabela.
    - vector<Ent>Ent: guarda informacoes sobre a entidade (nome e tamanho)
*/
class MetaData{
private:

    /*sem_t semaforo: TODO em proximas edições? */

    fstream streamTable;//ponteiro para leitura de arquivos
    string _tablename;
    size_t _totalRows;  //define o total e linha de dados da Tabela
    size_t _headerOffset;
    v_entity _entity;  // Classificacao do tipo de Dados
    // Insiro os dados como Nome aqui tambem ? 

    void OpenTable(bool is_readMode = false);
    void CloseTable();

    public: 
    
    // To Create a new table
    MetaData(string TableName, v_entity &Properties);

    // To Read a already constructed Table
    MetaData(string TableName);

    // MetaData(const string &FileName);
    ~MetaData();

    void write(); // escreve no arquivo de table
    void read();
    void Delete();


    string   get_name(bool fullName = false) const; // retorna o nome da tabela
    size_t   get_offset_row(size_t idx) const;      // retorna o valor de offset para um idx
    size_t   get_numRows() const;
    v_entity get_entities() const;                  // copia as propriedades de entidade
    size_t   get_offset() const;                    // retorna o total de offset para o final do arquivo
    size_t   get_sizeHeader() const;                //retorna o tamanho do cabeçalho(info de metadados)



    void set_name(string Name){_tablename = Name;};
    void set_entities(v_entity entity){_entity = entity;};
    void set_totalRows(size_t rows){_totalRows = rows;};
    void set_headerOffset(size_t _headerOffset){_headerOffset = _headerOffset;};

    
     //retorna o total de entidades do banco
    size_t total_entities() const {return _entity.size();}
    int get_rowOffset() const;         //retorna o comprimento de uma linha de dados


    //incrementa em mais um o total de elementos no metadado
    void increase_elements(){this->_totalRows+=1;};
    
    //reduz em menos um o total de elementos
    void decrease_elements(){this->_totalRows = min( (size_t) 0, (_totalRows - 1) );};
};

#endif /*_METADATA_H_*/