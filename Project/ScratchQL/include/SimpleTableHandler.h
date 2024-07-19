#ifndef SIMPLETABLEHANDLER_H
#define SIMPLETABLEHANDLER_H

#include <string>
#include <mutex>
#include <vector>
#include <array>
// #include <array>
#include <fstream>
#include <map>
#include <iomanip>
#include "HeaderHandler.h"
#include "DataInterface.h"
#include "FloatHandler.h"
#include "IntHandler.h"
#include "StringHandler.h"
#include "DateHandler.h"


using std::mutex, std::multimap, std::array;
using std::ios, std::to_string;

#define SIZE_TABLE_BUFFER (100)
#define MAX_DISPLAY_ROW (10)


DataInterface *dt_alloc(DataType type, string data="")
{
    DataInterface *ptr = nullptr;

    switch (type){
        case DataType::DATE:{
            ptr = new DateHandler();
            break;
        }
        case DataType::INT:{
            ptr = new IntHandler();
            break;
        }

        case DataType::STRING:{
            ptr = new StringHandler();
            break;
        }

        case DataType::FLOAT:{
            ptr = new FloatHandler();
            break;
        }

        case DataType::TYPE_NULL:
        default:
            break;
    }
    if(ptr!=nullptr && data!="")
        ptr->parseString(data);

    return ptr;
}

size_t data_size(DataType type){
    switch (type){
        case DataType::DATE:{
            DateHandler d;
            return d.bin_size();
        }
        case DataType::INT:{
            IntHandler d;
            return d.bin_size();
        }
        case DataType::STRING:{
            StringHandler d;
            return d.bin_size();
        }
        case DataType::FLOAT:{
            FloatHandler d;
            return d.bin_size();
        }

        case DataType::TYPE_NULL:
        default:
            break;
    }

    throw std::runtime_error("[ERROR] Fail to figure out which type is");
}
//just for notation - deal with it later
typedef string key_format;


class SimpleTableHandler
{
private:
    string Filename;

    std::fstream file_ptr;


    const int total_entities; //fixed value = 4

    // HeaderHandler header; //Not at this Moment

    //Store: p_key, s_key[0], s_key[1], s_key[2], s_key[3], s_key[4]
    multimap<key_format, size_t> map[4];


    vector<EntityProperties> prop;

    EntityProperties *primary_key;


public:

    SimpleTableHandler(std::string Filename);
    // SimpleTableHandler(HeaderHandler &Header);

    ~SimpleTableHandler(){};

    bool valid_pkey(key_format pkey);

    vector<DataInterface*> read_pkey(key_format pkey);
    // bool valid_skey(key_format skey);

    void read_file();
    // void write_file();


    void display();

    size_t total_items();
    size_t bin_fsize();
    size_t row_offset();    

    EntityProperties get_entity(string name_entity) const;

    void write_row(vector<DataInterface*> row);
    vector<DataInterface*> read_row(size_t row, bool is_RNN = false);

    bool check_write_row(vector<DataInterface*> row);


};

SimpleTableHandler::SimpleTableHandler(std::string Filename): Filename(Filename), total_entities(4)
{
    EntityProperties prop1("CPF"   , DataType::STRING, 0);
    EntityProperties prop2("NOME"  , DataType::STRING, 1);
    EntityProperties prop3("IDADE" , DataType::INT   , 2);
    EntityProperties prop4("CIDADE", DataType::STRING, 3);

    prop.push_back(prop1);
    prop.push_back(prop2);
    prop.push_back(prop3);
    prop.push_back(prop4);
    
    //set CPF as primary key
    primary_key = &prop1;
    std::cout << primary_key->idx_col;
    // *primary_key = prop1;

}

void SimpleTableHandler::write_row(vector<DataInterface*> row)
{
    file_ptr.open(Filename.c_str(), ios::binary | ios::app);


    for(int it = 0;it < row.size(); it++)
        row[it]->fwrite(file_ptr);

    file_ptr.close();
}

vector<DataInterface*> SimpleTableHandler::read_row(size_t idx, bool is_RRN)
{

    if (!file_ptr.is_open()) {
        file_ptr.open(Filename.c_str(), ios::binary | ios::in);
        // std::cerr << "[ERROR] Failed to open file\n";
        // return vector<DataInterface*>();
    }

    file_ptr.seekg(idx* (is_RRN ? 1 : row_offset()), ios::beg);

    vector<DataInterface*> row(total_entities, nullptr);

    StringHandler strhandler;
    IntHandler ihandler;

    for(int it = 0;it < total_entities; it++)
    {
        row[it] = dt_alloc(prop[it].type);
        row[it]->fread(file_ptr);
    }
    
    file_ptr.close();

    return row;
}

EntityProperties SimpleTableHandler::get_entity(string name_entity) const{
    
    EntityProperties ans;
    for(auto ptr: prop)
        if(ptr.name == name_entity) return ptr;
    throw std::runtime_error("[ERROR] File not opened, check if file really exists.");
}
void SimpleTableHandler::display(){
    

    vector<DataInterface*> row;
    vector<vector<DataInterface*>> vis_table;
    vector<size_t> size_ent(total_entities, 0);
   
    size_t total = total_items();
    size_t total2print = std::min((size_t)MAX_DISPLAY_ROW, total);

    auto pretty_sep = [&] () {
        for(int i = 0; i < size_ent.size(); i++)
            std::cout << '+' << std::setfill('-') << std::setw(size_ent[i]+2) << "";
        std::cout << "+\n";
        // std::cout<< std::setfill('-') << std::setw(1) << "+" << std::endl;
    };

    auto pretty_topic = [&] () {
        pretty_sep();
        for(int i = 0; i < size_ent.size(); i++)
            std::cout << '|'<< std::setfill(' ') << std::setw(size_ent[i]+2) << prop[i].name;
        std::cout << "|\n";
        pretty_sep();
    };

    auto pretty_line = [&] (int idx) {
        for(int i = 0; i < size_ent.size(); i++)
            std::cout << '|'<< std::setfill(' ') << std::setw(size_ent[i]+2) << vis_table[idx][i]->toString();
        std::cout << "|\n";
    };

    for(int i=0; i < size_ent.size();i++) size_ent[i] = std::max(size_ent[i], prop[i].name.size()+2);

    int idx =0;
    //Collect data
    while(idx < total2print){

        row = read_row(idx++);
        if(row.size()!= total_entities) break;
        
        vis_table.push_back(row);

        for(int i=0; i < total_entities;i++) {
            size_ent[i] = std::max(size_ent[i], (row[i]->toString()).size()); 
        }
    }

    //Print Data
    pretty_topic();    
    int i = std::max((size_t) 0, total2print);
        while(i--) pretty_line(i);
    pretty_sep();
    std::cout << "Total Rows: "<< total << std::endl;  
}

void SimpleTableHandler::read_file() // TODO: fix it!
{   
    if(!table_exists(Filename)){
        std::cerr << "[ERROR] File dont exist!\n";
    }
    size_t idx = 0;
    int total = total_entities;
    const size_t offset = row_offset();
    size_t fsize =bin_fsize();
    size_t RRN =0;

    std::cout << "[READ_FILE]\n";
    std::cout << "fsize: " << fsize << std::endl;
    std::cout << "total: " << total << std::endl;
    std::cout << "offset: " << offset << std::endl;

    file_ptr.open(Filename.c_str(), ios::binary | ios::in);

    DataInterface *dt = nullptr;
    while(RRN < fsize){

        if(file_ptr.tellg() == file_ptr.fail())
            std::cout << "[ERROR] File error\n";

        std::cout << "tellg: " << file_ptr.tellg() << std::endl;
        std::cout << "RRN "  << RRN <<std::endl;

        vector<DataInterface*> row = read_row(RRN, true);

        for(int iCol = 0; iCol < total_entities; iCol++) 
            map[iCol].insert({row[iCol]->toString(), RRN});

        std::cout << "RRN : ";
        for(auto itr : row) std:: cout <<itr->toString() << ", ";
        std::cout << "\n";

        RRN += offset;
        
    }
}



size_t SimpleTableHandler::row_offset(){
    
    size_t ans = 0;
    for(auto ptr : prop) ans += data_size(ptr.type);
    return ans;
}


size_t SimpleTableHandler::total_items(){

    file_ptr.open(Filename.c_str(), std::ios::binary|std::ios::in|std::ios::ate);

    if(file_ptr.good()) {
        std::cout << "row_offset:" << row_offset()<< std::endl;
        std::cout << "bin_fsize:" << bin_fsize()  << std::endl;
        size_t size = bin_fsize()/row_offset();
        file_ptr.close();

        return size;
    }

    perror(Filename.c_str());


    return -1;
}
size_t SimpleTableHandler::bin_fsize(){

    file_ptr.open(Filename.c_str(), std::ios::binary|std::ios::in|std::ios::ate);
    std::fstream::pos_type size = file_ptr.tellg();

    file_ptr.close();
    return  size;
}


bool SimpleTableHandler::valid_pkey(key_format key){
    
    int entity_idx = 0;

    if(map[entity_idx].find(key) == map[entity_idx].end())
        return false;
    return true;
}

vector<DataInterface*> SimpleTableHandler::read_pkey(key_format key){

    if(!valid_pkey(key))
        return vector<DataInterface*>(total_entities, nullptr);

    int p_idx = primary_key->idx_col;
    

    size_t RRN = map[0].find(key)->second;

    return read_row(RRN, true);    
}




#endif /*SIMPLETABLEHANDLER_H*/