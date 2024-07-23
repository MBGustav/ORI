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
#include <utility>
#include "HeaderHandler.h"
#include "DataInterface.h"
#include "FloatHandler.h"
#include "IntHandler.h"
#include "StringHandler.h"
#include "DateHandler.h"


using std::pair, std::multimap, std::array;
using std::ios, std::to_string;

#define SIZE_TABLE_BUFFER (100)
#define MAX_DISPLAY_ROW (15)


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

    // HeaderHandler header; //Not at this Moment
    string Filename;
    std::fstream file_ptr;
    bool temp_table;
    size_t items_added;
    const int total_entities; //fixed value = 4

    //Store: p_key, s_key[0], s_key[1], s_key[2], s_key[3], s_key[4]
    multimap<key_format, size_t> map[4];

    EntityProperties *primary_key;

    vector<EntityProperties> prop;



public:

    SimpleTableHandler(std::string Filename, bool temporary = false);

    SimpleTableHandler(EntityProperties &prop_instance, std::string Filename, bool is_tmp = true); //create a new table

    SimpleTableHandler(vector<vector<DataInterface*>>, std::string fname = "temp", bool is_tmp = true); //result from a query

    ~SimpleTableHandler(){ if(temp_table) std::remove(Filename.c_str());};

    bool valid_pkey(key_format pkey);
    bool valid_skey(key_format pkey, string name_entity);

    void write_row(vector<DataInterface*> row);

    vector<DataInterface*> read_row(size_t row, bool is_RNN = false); 


    vector<DataInterface*> read_pkey(key_format pkey);
    vector<vector<DataInterface*>> read_skey(key_format pkey, string name_entity);
    vector<vector<DataInterface*>> read_skey_greater(key_format key, std::string Name, bool get_equal= true);



    void display();
    void read_file();
    

    size_t total_items();
    size_t bin_fsize();
    size_t row_offset();    

    EntityProperties get_entity(string name_entity) const;
};


//Hard-coded Example prop
SimpleTableHandler::SimpleTableHandler(std::string Filename, bool temporary): Filename(Filename), total_entities(4), temp_table(temporary)
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

    //items added 
    items_added=0;
}


SimpleTableHandler::SimpleTableHandler(vector<vector<DataInterface*>> table, std::string fname, bool is_tmp )
                    :Filename(fname), temp_table(is_tmp), total_entities(4)
{  
    //create file
    file_ptr.open(Filename, std::ios::binary | std::ios::out );

    //copy header
    EntityProperties prop1("CPF"   , DataType::STRING, 0);
    EntityProperties prop2("NOME"  , DataType::STRING, 1);
    EntityProperties prop3("IDADE" , DataType::INT   , 2);
    EntityProperties prop4("CIDADE", DataType::STRING, 3);
    prop.push_back(prop1);
    prop.push_back(prop2);
    prop.push_back(prop3);
    prop.push_back(prop4);


    //write to file
    for(auto line: table){
        for(auto cell : line){
            cell->fwrite(file_ptr);
        }
        
    }
    
    //free space
    for(auto line: table)
        for(auto cell : line)
            delete cell;

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

void SimpleTableHandler::read_file()
{   
    if(!table_exists(Filename))
        std::cerr << "[ERROR] File dont exist!\n";
    
    size_t idx = 0;
    int total = total_entities;
    const size_t offset = row_offset();
    size_t fsize =bin_fsize();
    size_t RRN =0;

    file_ptr.open(Filename.c_str(), ios::binary | ios::in);

    DataInterface *dt = nullptr;
    while(RRN < fsize){
        vector<DataInterface*> row = read_row(RRN, true);
        for(int iCol = 0; iCol < total_entities; iCol++) 
            map[iCol].insert({row[iCol]->toString(), RRN});
        RRN += offset;    
    }
}


size_t SimpleTableHandler::row_offset(){    
    size_t ans = 0;
    for(auto ptr : prop) ans += data_size(ptr.type);
    return ans;
}


size_t SimpleTableHandler::total_items(){


    if(!file_ptr.good()) {
        perror(Filename.c_str());
        std::cerr <<"error to open\n";
        return -1;
    }

    size_t bin = bin_fsize();
    size_t offset = row_offset();
    // std::cout<<"bin: " << bin_fsize() <<", " << row_offset()<< "\n"; 
    size_t size = (size_t) bin_fsize()/row_offset();
    return size;
}


size_t SimpleTableHandler::bin_fsize(){

    file_ptr.open(Filename.c_str(), std::ios::binary|std::ios::in);
    // file_ptr.seekg(0,std::ios::end);
    // size_t size = file_ptr.tellg();
    // ifstream myfile ("example.bin", ios::binary);
    std::streampos begin = file_ptr.tellg();
    file_ptr.seekg (0, ios::end);
    std::streampos end = file_ptr.tellg();
    // std::cout << "size is: " << (end-begin) << " bytes.\n";
    file_ptr.close();
    return  end-begin;
}


bool SimpleTableHandler::valid_pkey(key_format key){
    
    int entity_idx = 0;//TODO:reader for primary key

    if(map[entity_idx].find(key) == map[entity_idx].end())
        return false;
    return true;
}


bool SimpleTableHandler::valid_skey(key_format key, string name_entity){
    
    int idx = get_entity(name_entity).idx_col;

    if(map[idx].find(key) == map[idx].end())
        return false;
    return true;
}



//only equal values
vector<DataInterface*> SimpleTableHandler::read_pkey(key_format key){

    if(!valid_pkey(key))
        return vector<DataInterface*>(total_entities, nullptr);

    int p_idx = primary_key->idx_col;
    

    size_t RRN = map[0].find(key)->second;

    return read_row(RRN, true);    
}

vector<vector<DataInterface*>> SimpleTableHandler::read_skey_greater(key_format key, std::string name_entity, bool add_equals)
{
    if(!valid_skey(key, name_entity)){
        std::cout << "[ERROR] sec. Key not found\n";
        return vector<vector<DataInterface*>>();
    }

    const int idx = get_entity(name_entity).idx_col;

    vector<size_t> RRN_list;
    vector<vector<DataInterface*>> table;

    //find the elements with the same key:
    pair <multimap<key_format,size_t>::iterator, multimap<key_format,size_t>::iterator> equal_range;

    multimap<key_format, size_t>::iterator  upper_range;

    //include equals
    if(add_equals) equal_range = map[idx].equal_range(key);

    //include uppers
    upper_range = map[idx].upper_bound(key);

    while(upper_range != map[idx].end()){//TODO: cast values(i.e float, int,..) to compare
        RRN_list.push_back(upper_range->second);
        upper_range++;
    }
    

    sort(RRN_list.begin(), RRN_list.end());

    file_ptr.open(Filename.c_str(), std::ios::binary | std::ios::in);


    for(auto rrn: RRN_list)
    {
        file_ptr.seekg(rrn, ios::beg);
        std::vector<DataInterface*> row;
        for(int i = 0; i < total_entities; i++){
            DataInterface* data_ptr = dt_alloc(prop[i].type);
            data_ptr->fread(file_ptr);
            row.push_back(data_ptr);
        }
        table.push_back(row);
    }

    file_ptr.close();


    return table;
}

vector<vector<DataInterface*>> SimpleTableHandler::read_skey(key_format key, string name_entity) // TODO: check values
{
    if(!valid_skey(key, name_entity)){
        std::cout << "[ERROR] sec. Key not found\n";
        return vector<vector<DataInterface*>>();
    }
    
    const int idx = get_entity(name_entity).idx_col;
    vector<vector<DataInterface*>> table;
    vector<DataInterface*> row;

    vector<size_t> RRN_list;

    //find the elements with the same key:
    pair <multimap<key_format,size_t>::iterator, multimap<key_format,size_t>::iterator> range;

    range = map[idx].equal_range(key);

    file_ptr.open(Filename.c_str(), std::ios::binary | std::ios::in);

    for(auto it = range.first; it != range.second; it++){
        RRN_list.push_back(it->second);
    }


    for(auto rrn: RRN_list)
    {
        file_ptr.seekg(rrn, ios::beg);
        std::vector<DataInterface*> row;
        for(int i = 0; i < total_entities; i++){
            DataInterface* data_ptr = dt_alloc(prop[i].type);
            data_ptr->fread(file_ptr);
            row.push_back(data_ptr);
        }
        table.push_back(row);
    }

    file_ptr.close();


    
    return table;
} 



#endif /*SIMPLETABLEHANDLER_H*/