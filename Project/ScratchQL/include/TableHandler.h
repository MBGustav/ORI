#ifndef TABLEHANDLER_H
#define TABLEHANDLER_H

#include <string>
#include <mutex>
#include <vector>
#include <array>
// #include <array>
#include <fstream>
#include <map>

#include "HeaderHandler.h"
#include "DataInterface.h"

#include "FloatHandler.h"
#include "IntHandler.h"
#include "StringHandler.h"
#include "DateHandler.h"

using std::mutex, std::multimap, std::array;


#define SIZE_TABLE_BUFFER (100)

//just for notation - deal with it later
typedef string primary_key;

class TableHandler
{
private:
    string Filename;
    // std::mutex &file_mtx;
    std::fstream file_ptr;

    //Primary Key properties
    map<string, EntityProperties> Entities;
    multimap<string, int> map;

    // std::multimap<int, string> mm;

    EntityProperties pkey_properties;

    //array containing the types of data
    HeaderHandler header;

    //structure for index up-down
        //First, the idx we're searching, then the entity
    vector<vector<DataInterface*>> idx_to_database;
    
    //structure for entities left-right || hash-chaining
    //ToDo: how to link both data structures ?

    bool idx_is_valid(size_t idx);

    size_t get_total_elements() const{return this->header.get_total_elements();};
    size_t get_offset_header()  const{return this->header.get_offset_header();};
    size_t get_offset_row()     const{return this->header.get_offset_row();};
    size_t get_total_entities() const{return this->header.get_total_entities();}

    vector<DataInterface*> get_by_idx(size_t idx);
    

public:

    TableHandler(std::string &Filename, std::mutex &file_mtx, map<string, EntityProperties> &Entities);
    TableHandler(HeaderHandler &Header);

    ~TableHandler();

    bool validate_pkey();
    void read_database();
    void write_database();
    vector<DataInterface*> read_row_idx(size_t idx);
    
    bool write_row_idx(size_t idx, vector<DataInterface>);

    vector<DataInterface*> get_row_pkey(primary_key pkey);
};

void TableHandler::read_database()
{
    file_ptr.open(Filename.c_str(), std::ios_base::binary | std::ios_base::in);
    size_t total_elements, r_offset, h_offset, total_entities;

    //read total_elements from binary file(offset = 0)
    total_elements = get_total_elements();

    // set offset row 
    r_offset = get_offset_row();
    
    // set offset header 
    h_offset = get_offset_header();

    total_entities = get_total_entities();

    // jump to data
    file_ptr.seekg(std::ios::beg + h_offset);

    idx_to_database.resize(total_elements); 
    
    for(size_t itr = 0; itr < total_elements && file_ptr ; itr++){
        
        vector<DataInterface> _row = get_by_idx(itr);
        idx_to_database[itr] = _row;
    }


}
void TableHandler::write_database(){

}

bool TableHandler::idx_is_valid(size_t idx){
    return true;
}
bool TableHandler::write_row_idx(size_t idx, vector<DataInterface>)
{
    if(!idx_is_valid(idx)){
        std::cerr << "[ERROR] idx does not valid!\n";
        return false; // Retorna um vetor vazio em caso de erro
    }

    

    size_t t_ent = get_total_entities();
    vector<DataInterface*> _row(t_ent);
    vector<EntityProperties> properties = header.get_EntityList();

    file_ptr.open(Filename.c_str(), std::ios::binary | std::ios::in);

    file_ptr.seekg(std::ios::beg + get_offset_header() + idx * get_offset_row());

    for (size_t i = 0; i < t_ent; ++i) {
        switch (properties[i].type) {
            case DataType::FLOAT: {
                FloatHandler float_data;
                file_ptr.read(reinterpret_cast<char*>(&float_data), sizeof(FloatHandler));
                _row[i] = reinterpret_cast<DataInterface*>(new FloatHandler(float_data));
                break;
            }

            case DataType::INT: {
                IntHandler int_data;
                file_ptr.read(reinterpret_cast<char*>(&int_data), sizeof(IntHandler));
                _row[i] = reinterpret_cast<DataInterface*>(new IntHandler(int_data));
                break;
            }

            case DataType::STRING: {
                StringHandler str_data;
                file_ptr.read(reinterpret_cast<char*>(&str_data), sizeof(StringHandler));
                _row[i] = reinterpret_cast<DataInterface*>(new StringHandler(str_data));
                break;
            }
            
            case DataType::DATE: {
                DateHandler date_data;
                file_ptr.read(reinterpret_cast<char*>(&date_data), sizeof(DateHandler));
                _row[i] = reinterpret_cast<DataInterface*>(new DateHandler(date_data));
                break;
            }

            default:{
                std::cerr << "[ERROR] Unknown Data type or not implemented yet!\n";
                break;
            }
        }
    }
    
    return _row;
}

vector<DataInterface*> TableHandler::read_row_idx(size_t idx)
{
    if(!idx_is_valid(idx)){
        std::cerr << "[ERROR] idx does not exist!\n";
        return vector<DataInterface*>(); // Retorna um vetor vazio em caso de erro
        
    }

    size_t t_ent = get_total_entities();
    vector<DataInterface*> _row(t_ent);
    vector<EntityProperties> properties = header.get_EntityList();

    file_ptr.open(Filename.c_str(), std::ios::binary | std::ios::in);

    file_ptr.seekg(std::ios::beg + get_offset_header() + idx * get_offset_row());

    for (size_t i = 0; i < t_ent; ++i) {
        switch (properties[i].type) {
            case DataType::FLOAT: {
                FloatHandler float_data;
                file_ptr.read(reinterpret_cast<char*>(&float_data), sizeof(FloatHandler));
                _row[i] = reinterpret_cast<DataInterface*>(new FloatHandler(float_data));
                break;
            }

            case DataType::INT: {
                IntHandler int_data;
                file_ptr.read(reinterpret_cast<char*>(&int_data), sizeof(IntHandler));
                _row[i] = reinterpret_cast<DataInterface*>(new IntHandler(int_data));
                break;
            }

            case DataType::STRING: {
                StringHandler str_data;
                file_ptr.read(reinterpret_cast<char*>(&str_data), sizeof(StringHandler));
                _row[i] = reinterpret_cast<DataInterface*>(new StringHandler(str_data));
                break;
            }
            
            case DataType::DATE: {
                DateHandler date_data;
                file_ptr.read(reinterpret_cast<char*>(&date_data), sizeof(DateHandler));
                _row[i] = reinterpret_cast<DataInterface*>(new DateHandler(date_data));
                break;
            }

            default:{
                std::cerr << "[ERROR] Unknown Data type or not implemented yet!\n";
                break;
            }
        }
    }

    return _row;
}

TableHandler::TableHandler(std::string &Filename, std::mutex &file_mtx, map<string, EntityProperties> &Entities):
            Filename(Filename), Entities(Entities), header(Filename)
{
    if(table_exists(Filename))
        read_database();
    else
        std::cerr << "[ERROR] DataBase not Existent!" << std::endl;
}


TableHandler:: TableHandler(HeaderHandler &Header): header(Header)
{

}

TableHandler::~TableHandler()
{
}



#endif /*TABLEHANDLER_H*/