#ifndef _METADATA_H_
#define _METADATA_H_ 

#include <iostream> 
#include <string> 
#include <vector>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <mutex>
#include <map>

// #include "ErrorHandler.h"
#include "DataInterface.h"
#include "HeaderHandler.h"


#define TABLE_EXT ".tab"
#define TEMP_TABLE "temp_table"
#define DIRECTORY_FOR_TABLES "FolderTables/"
#define TOTAL_DATA_DISPLAY (12)

using std::fstream, std::mutex, 
      std::vector , std::map;

vector<string> ListTables();


typedef enum open_mode{
    APPEND, 
    WRITE_NEW,
    READ_ONLY,
    WRITE_START,
    READ_FROM_TABLE
}open_mode;



class MetaData{
private:


    fstream file_ptr;  //ponteiro para leitura de arquivos
    mutex file_mtx;

    size_t header_offset;
    size_t items_added;

    HeaderHandler _header;
    // TableHandler _table;

    

public:
    ~MetaData();
    MetaData(string Filename); // open an existing file.tab, or create it
    
    MetaData(); // create an auxiliar tab, with name "temp_tab_[i].tab"

    void open_metadata();
    void create_metadata();

    
    void read_header();
    void update_header();

    void read_table();
    void update_table();


    void valid_idx(size_t idx);   // if not available, raise exception
    bool valid_fkey(string fkey); // if not available, raise exception
    bool valid_pkey(string pkey); // if not available, raise exception(auto generate = [int++ ])
    bool valid_data_row(vector<DataInterface> _row);


    size_t total_items();

    void display(size_t start=0, size_t end= (size_t) TOTAL_DATA_DISPLAY);

    size_t get_header_offset() const {return header_offset;};
    size_t get_items_added() const   {return items_added;};
    void set_header_offset(size_t header_offset){this->header_offset = header_offset;};


};


MetaData::MetaData(string Filename = TEMP_TABLE): _header(Filename, file_mtx){

}

    
void MetaData::open_metadata(){

}
void MetaData::create_metadata(){

}


void MetaData::read_header(){

}
void MetaData::update_header(){

}

void MetaData::read_table(){

}
void MetaData::update_table(){

}


void MetaData::valid_idx(size_t idx){

}
bool MetaData::valid_fkey(string fkey){

}
bool MetaData::valid_pkey(string pkey){

}
bool MetaData::valid_data_row(vector<DataInterface> _row){

}


size_t MetaData::total_items(){

}

void MetaData::display(size_t start, size_t end){

}
#endif /*_METADATA_H_*/