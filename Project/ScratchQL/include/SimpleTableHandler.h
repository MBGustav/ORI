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

    // size_t get_total_elements() const{return this->header.get_total_elements();};
    // size_t get_offset_header()  const{return this->header.get_offset_header();};
    // size_t get_offset_row()     const{return this->header.get_offset_row();};
    // size_t get_total_entities() const{return this->header.get_total_entities();}
    

public:

    SimpleTableHandler(std::string Filename);
    // SimpleTableHandler(HeaderHandler &Header);

    ~SimpleTableHandler(){};

    // bool valid_pkey(key_format pkey);
    // bool valid_skey(key_format skey);

    void read_file();
    // void write_file();


    void display();

    size_t total_items();
    size_t bin_fsize();
    size_t row_offset();    

    void write_row(vector<DataInterface*> row);
    vector<DataInterface*> read_row(size_t row);

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

}


void SimpleTableHandler::write_row(vector<DataInterface*> row)
{
    file_ptr.open(Filename.c_str(), ios::binary | ios::app);


    for(int it = 0;it < row.size(); it++)
    {
        row[it]->fwrite(file_ptr);
    }

    file_ptr.close();
}

vector<DataInterface*> SimpleTableHandler::read_row(size_t idx)
{
    file_ptr.open(Filename.c_str(), ios::binary | ios::in);

    if (!file_ptr.is_open()) {
        std::cerr << "[ERROR] Failed to open file\n";
        return vector<DataInterface*>();
    }

    file_ptr.seekg(idx*row_offset(), ios::beg);

    // std::cout << "offset: " << row_offset() << std::endl;
    // std::cout << "tellg:  " << file_ptr.tellg() << std::endl;
    // std::cout << "RRN: " << idx*row_offset() << std::endl;
    
    vector<DataInterface*> row(total_entities);

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


void SimpleTableHandler::display(){
    

    vector<DataInterface*> row;
    vector<vector<DataInterface*>> vis_table;
    vector<size_t> size_ent(total_entities, 0);

    

    size_t total = total_items();

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
    while(idx < std::min((size_t)MAX_DISPLAY_ROW, total)){

        row = read_row(idx++);
        if(row.size()!= total_entities) break;

        vis_table.push_back(row);

        for(int i=0; i < total_entities;i++) {
            size_ent[i] = std::max(size_ent[i], (row[i]->toString()).size()); 
        }
    }
    
    //Print Data
    
    pretty_topic();    
    int i = std::min((size_t) MAX_DISPLAY_ROW, total);
        while(i--) pretty_line(i);
    pretty_sep();

    std::cout << "Total Rows: "<< total << std::endl;  
}

void SimpleTableHandler::read_file()
{   
    if(!table_exists(Filename)){
        std::cerr << "[ERROR] File dont exist!\n"; 
    }
    
    const size_t offset = row_offset();
    size_t idx = 0;
    IntHandler ihandler;
    StringHandler strhandler;
    
    file_ptr.open(Filename.c_str(), ios::binary | ios::out);

    while(file_ptr)
    {
        size_t RRN = idx*offset;

        for(int i =0; i < 4 ; i++){
            switch (prop[i].type){
                case DataType::STRING: {
                    strhandler.fread(file_ptr);
                    map[i].insert({strhandler.toString(), RRN});
                }
                case DataType::INT: {
                    ihandler.fread(file_ptr);
                    map[i].insert({ihandler.toString(), RRN});
                }
                default: 
                    std::cerr << "[ERROR] Data Format not recognized\n";
            }
        }
        idx++;
    }
    
}



size_t SimpleTableHandler::row_offset(){
    IntHandler h;
    StringHandler f;
    return 3*f.bin_size() + h.bin_size();
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
    std::cout << "size: " << size <<std::endl;

    file_ptr.close();
    return  size;
}


#endif /*SIMPLETABLEHANDLER_H*/