#ifndef SIMPLETABLEHANDLER_H
#define SIMPLETABLEHANDLER_H

#include <string>
#include <mutex>
#include <vector>
#include <array>
#include <fstream>
#include <map>
#include <iomanip>


#include "FileHandler.h"
#include "DataInterface.h"
#include "FloatHandler.h"
#include "IntHandler.h"
#include "StringHandler.h"
#include "DateHandler.h"


using std::pair, std::multimap, std::array;
using std::ios, std::to_string;

#define MAX_DISPLAY_ROW (10)

DataType str2type(string type) {
    if(type == "STRING") return DataType::STRING;
    if(type == "INT")    return DataType::INT;
    if(type == "DATE")   return DataType::DATE;
    if(type == "FLOAT")  return DataType::FLOAT;
    
    return TYPE_NULL;
}

string type2str(DataType type) {
    if(type == DataType::FLOAT)  return string("FLOAT");
    if(type == DataType::STRING) return string("STRING");
    if(type == DataType::INT)    return string("INT");
    if(type == DataType::DATE)   return string("DATE");
    return string("TYPE_NULL");
}

typedef string key_format;

class SimpleTableHandler //TODO: renomear classe para - SQLTable
{
private:
    string Filename;    
    FileHandler file_handler;
    multimap<key_format, size_t> map[10]; // TODO: Expand for more maps
    vector<EntityProperties> *prop;

    void open(); 


public:

    SimpleTableHandler(SimpleTableHandler&&) noexcept = default;
    SimpleTableHandler& operator=(SimpleTableHandler&&) noexcept = default;

    SimpleTableHandler(const SimpleTableHandler&) = delete;
    SimpleTableHandler& operator=(const SimpleTableHandler&) = delete;
    
    void close(){file_handler.close();};
    void write_row(vector<DataInterface*> row);
    
    //%%%%%%%%%%% -- CONSTRUCTORS -- %%%%%%%%%%%
    SimpleTableHandler(std::string filename,        
                       bool temporary = false);
    SimpleTableHandler(std::string filename,                
                       vector<EntityProperties> properties, 
                       bool temporary = true); 
    SimpleTableHandler(vector<vector<DataInterface*>> table,
                       vector<EntityProperties> &prop,
                       std::string fname,
                       bool is_tmp );


    ~SimpleTableHandler(){file_handler.close();};

    //validations
    bool valid_pkey(key_format pkey);
    bool valid_skey(key_format pkey, string name_entity);

    //manipulations - row
    vector<DataInterface*> read_row(size_t row, bool is_RNN = false); 
    vector<DataInterface*> read_pkey(key_format pkey);

    //getter
    size_t get_total_elements() const {return file_handler.get_total_elements();}
    size_t get_total_entities() const {return file_handler.get_total_entities();}
    vector<EntityProperties> get_entities() const {return vector<EntityProperties>(file_handler.get_entities());}
    EntityProperties get_entity(string name_entity) const;
    vector<string> get_entity_names() const;
    int get_entity_idx(string name_entity) const;

    //setter
    void set_entities(vector<EntityProperties> vec);

    //manipulations - file
    void read_file();

    //TODO: fix it, maybe returns an another Table ?
    vector<vector<DataInterface*>> read_skey(key_format pkey, string name_entity);
    vector<vector<DataInterface*>> read_skey_greater(key_format key, std::string Name, bool get_equal= true);

    //status table info.
    size_t total_items();
    size_t bin_fsize();
    size_t row_offset();    
    void display();

    static void delete_table(string table_name);

    // Data Definition Language (DDL)
    void alter();   // under construction
    void drop();    // will delete your whole table, including all data,
    void truncate();// Remove all data entries from a table while keeping structure

    // Data Manipulation Language (DML)
    bool insert(vector<string> row);                // adding a row of data  (safe mode)
    bool insert(vector<DataInterface*> row);        // same typo             (safe mode)
    bool insert(vector<vector<string>> row);        // to add multiple rows  (safe mode)
    bool delete_pk(key_format primary_key);         // remove the row by id  (or PK)
    bool update(key_format pkey,string new_value);  // change values by pkey


    // Transaction Control Language (TCL) i guess i cant...
    void savepoint(); // Is it really necessary? 
    void commit();    // Damn, thats hard
    void rollback();  // I simply cant

    // Data Query Language (DQL)
    // This will be done in a SQLManager

};

SimpleTableHandler::SimpleTableHandler(std::string filename, vector<EntityProperties> properties, bool temporary):
        Filename(filename), file_handler(filename, properties, temporary)
{
    set_entities(properties);
}

vector<string> SimpleTableHandler::get_entity_names() const{
        vector<string> entities_name;
        for(auto itr : file_handler.get_entities())
            entities_name.push_back(itr.name);
        return entities_name;
    }

SimpleTableHandler::SimpleTableHandler(std::string filename, bool temporary):
        Filename(filename), file_handler(filename, temporary)
{
    open();
}

SimpleTableHandler::SimpleTableHandler(vector<vector<DataInterface*>> table,vector<EntityProperties> &prop, std::string fname, bool is_tmp )
                    :Filename(fname), file_handler(Filename, is_tmp)
{  
    
    //set entity prop.
    file_handler.set_entities(prop);

    //write to file
    for(auto line: table){
        file_handler.write_data(line);
    }
    
    //free space
    for(auto line: table)
        for(auto cell : line)
            delete cell;
}


void SimpleTableHandler::open()
{
    //read from header.bin
    file_handler.open(false);

    //get from header
    set_entities(file_handler.get_entities());
    read_file();
}

void SimpleTableHandler::set_entities(vector<EntityProperties> vec){
        if(vec.data() == nullptr || vec.size() <= 0)
            throw std::runtime_error("[ERROR] Fail to Read Entities\n");        
        prop = &vec;
    }
void SimpleTableHandler::write_row(vector<DataInterface*> row)
{
    file_handler.write_data(row);
}

vector<DataInterface*> SimpleTableHandler::read_row(size_t idx, bool is_RRN)
{
    vector<DataInterface*> row = file_handler.read_row(idx, is_RRN);

    for(auto r : row) 
        if(!r) throw std::runtime_error("[ERROR] Null access");

    return row;
}

EntityProperties SimpleTableHandler::get_entity(string name_entity) const{
    
    EntityProperties ans;
    for(auto ptr: *prop)
        if(ptr.name == name_entity) return ptr;
        
    throw std::runtime_error("[ERROR] Entity Not Found.");
}


int SimpleTableHandler::get_entity_idx(string name_entity) const{
    EntityProperties ans;
    for(auto ptr: *prop)
        if(ptr.name == name_entity) return ptr.idx_col;
    throw std::runtime_error("[ERROR] Entity Not Found.");
}

void SimpleTableHandler::display(){
    

    vector<DataInterface*> row;
    vector<vector<DataInterface*>> vis_table;
    vector<size_t> size_ent(get_total_entities(), 0);
    vector<string> entity_name = get_entity_names();
   
    size_t total = total_items();
    size_t total2print = std::min((size_t)MAX_DISPLAY_ROW, total);



    auto pretty_sep = [&] () {
        for(int i = 0; i < size_ent.size(); i++)
            std::cout << '+' << std::setfill('-') << std::setw(size_ent[i]+2) << "";
        std::cout << "+\n";
    };

    auto pretty_topic = [&] () {
        pretty_sep();
        for(int i = 0; i < size_ent.size(); i++)
            std::cout << '|'<< std::setfill(' ') << std::setw(size_ent[i]+2) << entity_name[i];
        std::cout << "|\n";
        pretty_sep();
    };

    auto pretty_line = [&] (int idx) {
        for(int i = 0; i < size_ent.size(); i++)
            std::cout << '|'<< std::setfill(' ') << std::setw(size_ent[i]+2) << vis_table[idx][i]->toString();
        std::cout << "|\n";
    };

    for(int i=0; i < size_ent.size();i++) size_ent[i] = std::max(size_ent[i], entity_name[i].size()+2);

    int idx =0;
    //Collect size to print
    while(idx < total2print){
        row = read_row(idx++);

        if(row.size()!= get_total_entities()) break;
        vis_table.push_back(row);
            

        for(int i=0; i < get_total_entities();i++) {
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

void SimpleTableHandler::read_file() // TODO: fix when we read a file, we must find a header
{   
    bool find = file_handler.read_header(file_handler.get_header_name());

    if(!find) throw std::runtime_error("[ERROR] Fail to Read Header\n");


    //read from data.bin
    size_t RRN = 0;
    while(RRN < file_handler.get_total_elements()){
        vector<DataInterface*> row = file_handler.read_row(RRN, true);
        if(row.size() != get_total_entities()) break;

        key_format key = row[0]->toString();
        map[0].insert(pair<key_format, size_t>(key, RRN));
        RRN++;
    }


}


size_t SimpleTableHandler::row_offset(){    
    // size_t ans = 0;
    // for(auto ptr : prop) ans += data_size(ptr.type);
    return file_handler.row_offset();
    // return ans;
}


size_t SimpleTableHandler::total_items()
{

    return file_handler.get_total_elements();
}
size_t SimpleTableHandler::bin_fsize(){
    return file_handler.bin_fsize();
}


bool SimpleTableHandler::valid_pkey(key_format key){
    
    int entity_idx = 0;
    
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



vector<DataInterface*> SimpleTableHandler::read_pkey(key_format key){

    if(!valid_pkey(key)){
        return vector<DataInterface*>(get_total_entities(), nullptr);
    }

    
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

    // file_ptr.open(Filename.c_str(), std::ios::binary | std::ios::in);

    vector<EntityProperties> vec_prop = get_entities();
    for(auto rrn: RRN_list)
    {
        file_handler.seek_data(ios::beg, rrn);

        std::vector<DataInterface*> row;
        for(int i = 0; i < get_total_entities(); i++){
            DataInterface* data_ptr = dt_alloc(vec_prop[i].type);
            file_handler.read_data(data_ptr);
            row.push_back(data_ptr);
        }
        table.push_back(row);
    }
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

    // file_ptr.open(Filename.c_str(), std::ios::binary | std::ios::in);

    for(auto it = range.first; it != range.second; it++){
        RRN_list.push_back(it->second);
    }

    vector<EntityProperties> vec_prop = get_entities();
    for(auto rrn: RRN_list)
    {
        file_handler.seek_data(ios::beg, rrn);

        std::vector<DataInterface*> row;
        for(int i = 0; i < get_total_entities(); i++){
            DataInterface* data_ptr = dt_alloc(vec_prop[i].type);
            file_handler.read_data(data_ptr);
            row.push_back(data_ptr);
        }
        table.push_back(row);
    }

    return table;
}


void SimpleTableHandler::delete_table(string table_name) {
    string folder_path = "ScratchQL\\FolderTables\\"; // Caminho da pasta
    string table_header = folder_path + table_name +  "_header.bin";
    table_name = folder_path + table_name + "_data.bin";


    if (std::filesystem::exists(table_name) && std::filesystem::exists(table_header)) {
        std::filesystem::remove(table_name);
        std::filesystem::remove(table_header);
        std::cout << "Tabela " << table_name << " deletada com sucesso." << std::endl;
    } else {
        std::cout << "Tabela " << table_name << " nao encontrada." << std::endl;
        std::cout << "Header" << table_header << " nao encontrada." << std::endl;
    }
}

bool SimpleTableHandler::insert(vector<string> row){
    //constraint
    int pkey_idx = 0;
    vector<EntityProperties> ent = get_entities();
    size_t sz = ent.size();
    size_t sz_row = row.size();

    if(sz_row !=  sz) return false;

    // check primary key - if is valid, already contains
    if(valid_pkey(row[pkey_idx])) return false;

    //data conversion - check if possible ?
    vector<DataInterface*>insert_row(sz);
    for(int i = 0; i < sz; i++){
        insert_row[i] = dt_alloc(ent[i].type, row[i]);
    }
    


    write_row(insert_row);

    return true;
}
bool SimpleTableHandler::insert(vector<DataInterface*> row){

    //constraint
    int pkey_idx = 0;
    vector<EntityProperties> ent = get_entities();
    size_t sz = ent.size();
    size_t sz_row = row.size();
    if(sz_row !=  sz) return false;
    
    //check memory consistency
    for(auto &c : row) if(c == nullptr) return false;
        
    // check primary key - if is valid, already contains
    if(valid_pkey(row[pkey_idx]->toString())) return false;
    
    write_row(row);
    return true; 
}
bool SimpleTableHandler::insert(vector<vector<string>> table){
    
    const size_t ent_size = get_entities().size();
    const int pkey_idx= 0;

    size_t itr_rows  = table.size();
    for(size_t r = 0; r<itr_rows; r++){
        if(table[r].size() != ent_size) return false;
        if(valid_pkey(table[r][pkey_idx])) return false;
    }

    for(auto &line: table)
        if(!this->insert(line)) return false;
    return true;
}
bool SimpleTableHandler::delete_pk(key_format primary_key){
    return false;
}
bool SimpleTableHandler::update(key_format pkey,string new_value){
    return false;
}

void SimpleTableHandler::drop(){//BE CAREFUL, THAT DELETES EVERYTHING! >:(

    file_handler.destroy();
}


#endif /*SIMPLETABLEHANDLER_H*/