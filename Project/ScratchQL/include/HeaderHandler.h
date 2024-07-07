#ifndef HEADERHANDLER_H_
#define HEADERHANDLER_H_ 

#include <iostream> 
#include <string> 
#include <cstring> /*memcpy*/
#include <vector>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <mutex>
#include <shared_mutex>
#include <syncstream>
#include <map>

// #include "ErrorHandler.h"
#include "DataInterface.h"

#define TABLE_EXT ".tab"
#define DIRECTORY_FOR_TABLES "FolderTables/"
#define TOTAL_DATA_DISPLAY (12)

using std::fstream, std::mutex,
      std::vector , std::map;

vector<string> ListTables();


typedef struct EntityProperties{
    string name;
    DataType type;
    
    EntityProperties(string _name="", DataType _type=TYPE_NULL): 
        name(_name), type(_type){};

}EntityProperties;


class HeaderHandler{
private:

    fstream file_ptr;
    // std::shared_mutex& fileMutex_; ??? TODO
    mutex &file_mtx_;
    string Filename;
    //melhor seria um array ou um map ? acesso por indice e por nome
    map<string, EntityProperties> Entities;

    size_t offset_header;
    size_t total_elements;
    size_t offset_row;

public:
    HeaderHandler(string &Filename, mutex &file_mtx_); //Constructor for already existent file
    ~HeaderHandler();

    void read_header();
    void write_header();

    //setters
    void set_total_elements(size_t total_elements) {this->total_elements = total_elements;};
    void set_offset_header(size_t offset_header)   {this->offset_header = offset_header;};
    void set_offset_row(size_t offset_row)         {this->offset_row = offset_row;};
    
    //getters
    size_t get_total_elements() const {return this->total_elements;};
    size_t get_total_entities() const {return Entities.size();};
    size_t get_offset_header() const  {return this->offset_header;};
    size_t get_offset_row() const     {return this->offset_row;};

    EntityProperties *get_EntityProperties(size_t idx);
    EntityProperties *get_EntityProperties(string name_entity); 
    
};

void HeaderHandler::write_header()
{
    file_ptr.open(Filename.c_str(), std::ios_base::binary | std::ios_base::out);
    size_t int_ptr=0, total_entities=0;
    size_t size_name;
    string name_ent;
    EntityProperties aux_entityProp();

    if(!file_ptr.is_open())
        throw std::runtime_error("[ERROR] File not opened, check if file really exists.");


    //read total elem.
    int_ptr = get_total_elements();
    file_ptr.write(reinterpret_cast<char*>(&int_ptr), sizeof(int_ptr));

    //read offset elem
    int_ptr = get_offset_row();
    file_ptr.write(reinterpret_cast<char*>(&int_ptr), sizeof(int_ptr));

    //read offset header
    int_ptr = get_offset_header();
    file_ptr.write(reinterpret_cast<char*>(&int_ptr), sizeof(int_ptr));

    //read total_ent
    total_entities = get_total_entities();
    file_ptr.write(reinterpret_cast<char*>(&total_entities), sizeof(total_entities));
    
    
    // iterate over all entitties types
    EntityProperties *datatype_arr = new EntityProperties[total_entities];
    
    
    size_t itr =0;
    for(auto it = Entities.begin(); it != Entities.end(); ++it){
        datatype_arr[itr++] = it->second;
    }
    
    for(size_t it =0; it < total_entities; it ++ ){
        file_ptr.write(reinterpret_cast<char*>(&(datatype_arr[it].type)), sizeof(DataType));
    }


    //iterate over all entities names
    for(itr = 0; itr < total_elements; itr++){
        name_ent = datatype_arr[itr].name;
        size_name = name_ent.size();
        
        file_ptr.write(reinterpret_cast<char*>(&size_name), sizeof(size_name));
        file_ptr.write(name_ent.c_str(), size_name);
    }

    delete[]  datatype_arr;
}


void HeaderHandler::read_header()
{
    file_ptr.open(Filename.c_str(), std::ios_base::binary | std::ios_base::out);
    size_t int_ptr=0, total_entities=0;
    size_t size_name;
    EntityProperties aux_entityProp();

    if(!file_ptr.is_open())
        throw std::runtime_error("[ERROR] File not opened, check if file really exists.");


    //read total elem.
    file_ptr.read(reinterpret_cast<char*>(&int_ptr), sizeof(int_ptr));
    set_total_elements(int_ptr);

    //read offset elem
    file_ptr.read(reinterpret_cast<char*>(&int_ptr), sizeof(int_ptr));
    set_offset_row(int_ptr);

    //read offset header
    file_ptr.read(reinterpret_cast<char*>(&int_ptr), sizeof(int_ptr));
    set_offset_header(int_ptr);

    //read total_ent
    file_ptr.read(reinterpret_cast<char*>(&total_elements), sizeof(total_elements));
    
    
    // iterate over all entitties types
    DataType *datatype_arr = new DataType[total_elements];

    // for(size_t itr = 0; itr < total_elements; itr++){
    file_ptr.read(reinterpret_cast<char*>(&datatype_arr[0]), total_elements * sizeof(DataType));
    // }

    //iterate over all entities names
    
    for(size_t itr = 0; itr < total_elements; itr++){
        file_ptr.read(reinterpret_cast<char*>(&size_name), sizeof(size_name));
        
        char *tmp = new char[size_name+1]; //to load the class
        file_ptr.read(&tmp[0], size_name);
        tmp[size_name] = '\0';

        Entities[tmp] = EntityProperties(tmp, datatype_arr[itr]);

        delete[] tmp;
    }

    delete []  datatype_arr;

}

HeaderHandler::HeaderHandler(string &Filename, mutex &file_mtx_): 
              Filename(Filename), file_mtx_(file_mtx_)
{
    read_header();
}

HeaderHandler::~HeaderHandler(){}

EntityProperties *HeaderHandler::get_EntityProperties(size_t idx)
{
    EntityProperties *copy_of = new EntityProperties;
    copy_of->name = ""; 
    copy_of->type = TYPE_NULL;
    //constraint
    if(idx >= Entities.size()) return copy_of;
    map<string, EntityProperties>::iterator *it;
    int i =0;
    for(*it = Entities.begin(); *it != Entities.end() && i<idx; it++)
        if(i == idx) std::memcpy(copy_of, it, sizeof(EntityProperties));
    
    return copy_of;
}

EntityProperties *HeaderHandler::get_EntityProperties(string name_entity)
{
    EntityProperties *copy_of = new EntityProperties;

    copy_of->name = ""; 
    copy_of->type = TYPE_NULL;

    map<string, EntityProperties>::iterator it = Entities.find(name_entity);

    if(it != Entities.end())
        std::memcpy(copy_of, &it, sizeof(EntityProperties));

    return copy_of;
}



#endif /*HEADERHANDLER_H_*/