#ifndef SIMPLETABLEHANDLER_H
#define SIMPLETABLEHANDLER_H

#include <string>
#include <mutex>
#include <vector>
#include <array>
#include <fstream>
#include <map>
#include <iomanip>
// #include <array>

#include "FileHandler.h"
#include "DataInterface.h"
#include "FloatHandler.h"
#include "IntHandler.h"
#include "StringHandler.h"
#include "DateHandler.h"


using std::pair, std::multimap, std::array;
using std::ios, std::to_string;

// typedef struct EntityProperties{
//     string name;
//     int idx_col;
//     DataType type;

//     //construtor
//     EntityProperties(string _name="", DataType _type=TYPE_NULL,size_t id_col=0): 
//         name(_name), type(_type),idx_col(id_col){};

// }EntityProperties;


// #define SIZE_TABLE_BUFFER (100)
#define MAX_DISPLAY_ROW (10)

//just for notation - deal with it later
typedef string key_format;


class SimpleTableHandler
{
private:
    string Filename;    
    FileHandler file_handler;
    multimap<key_format, size_t> map[10]; // TODO: Expand for more maps
    vector<EntityProperties> *prop;


    // bool valid_insertion(vector<DataInterface*> row){
    //     if(row.size() != get_total_entities()) return false;
    //     vector<EntityProperties> entity = file_handler.get_entities();
    //     for(int i = 0; i < row.size(); i++) if(row[i]->read_DataType() != entity) return false;
    //     return true;
    // }
    void open(); 
    void update();

public:
    void close(){file_handler.close();};
    
    //read a file
    SimpleTableHandler(std::string filename,        
                       bool temporary = false);

    //force a file creation
    SimpleTableHandler(std::string filename,                
                       vector<EntityProperties> properties, 
                       bool temporary = true); 


    SimpleTableHandler(vector<vector<DataInterface*>>, 
                       std::string fname = "temp", 
                       bool is_tmp = true); //result from a query


    ~SimpleTableHandler(){file_handler.close();};

    //validations
    bool valid_pkey(key_format pkey);
    bool valid_skey(key_format pkey, string name_entity);

    //manipulations - row
    void write_row(vector<DataInterface*> row);
    vector<DataInterface*> read_row(size_t row, bool is_RNN = false); 
    vector<DataInterface*> read_pkey(key_format pkey);

    //getter
    size_t get_total_elements() const {return file_handler.get_total_elements();}
    size_t get_total_entities() const {return file_handler.get_total_entities();}
    vector<EntityProperties> get_entities() const {return vector<EntityProperties>(file_handler.get_entities());}

    //setter
    void set_entities(vector<EntityProperties> vec){
        if(vec.data() == nullptr || vec.size() <= 0)
            throw std::runtime_error("[ERROR] Fail to Read Entities\n");        
        prop = &vec;
    }

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
    int get_entity_idx(string name_entity) const;
    EntityProperties get_entity(string name_entity) const;
    
    vector<string> get_entity_names() const{
        vector<string> entities_name;
        for(auto itr : file_handler.get_entities())
            entities_name.push_back(itr.name);
        return entities_name;
    }
};

SimpleTableHandler::SimpleTableHandler(std::string filename, vector<EntityProperties> properties, bool temporary):
        Filename(filename), file_handler(filename, properties, temporary)
{

    
    set_entities(properties);

    
    // file_ptr.open(Filename.c_str(), std::ios::out);
    // file_ptr.close();
}

//Hard-coded Example prop or check for existent file
SimpleTableHandler::SimpleTableHandler(std::string filename, bool temporary):
        Filename(filename), file_handler(filename, temporary)
{
    open();

    // vector<EntityProperties> prop = {
    //     EntityProperties("CPF"   , DataType::STRING, 0),
    //     EntityProperties("NOME"  , DataType::STRING, 1),
    //     EntityProperties("IDADE" , DataType::INT   , 2),
    //     EntityProperties("CIDADE", DataType::STRING, 3)
    // };
    // file_handler.set_entities(prop);

    //set CPF as primary key

    // primary_key = &prop[0];

}

SimpleTableHandler::SimpleTableHandler(vector<vector<DataInterface*>> table, std::string fname, bool is_tmp )
                    :Filename(fname), file_handler(Filename, is_tmp)
{  
    
    vector<EntityProperties> prop = {
        EntityProperties("CPF"   , DataType::STRING, 0),
        EntityProperties("NOME"  , DataType::STRING, 1),
        EntityProperties("IDADE" , DataType::INT   , 2),
        EntityProperties("CIDADE", DataType::STRING, 3)
    };

    //copy header
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
    bool find = list_tables().count(file_handler.get_data_name());


    const size_t offset = row_offset();
    size_t fsize = bin_fsize();
    size_t RRN = 0;
    
    DataInterface *dt = nullptr;
    while(RRN < fsize){
        if(!file_handler.eof_data()) break;
        vector<DataInterface*> row = read_row(RRN, true);
        for(int iCol = 0; iCol < get_total_entities(); iCol++) 
            map[iCol].insert({row[iCol]->toString(), RRN});
        RRN += offset;
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
    std::cout << "map-size: " << map[entity_idx].size()<<"\n";
    
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

    if(!valid_pkey(key))
        return vector<DataInterface*>(get_total_entities(), nullptr);
    

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
    // file_ptr.close();

    return table;
} 



#endif /*SIMPLETABLEHANDLER_H*/