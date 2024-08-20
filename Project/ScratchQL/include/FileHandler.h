#ifndef FILEHANDLER_H_
#define FILEHANDLER_H_ 

#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <set>

#include "HeaderHandler.h"
#include "DataInterface.h"
#include "FloatHandler.h"
#include "IntHandler.h"
#include "StringHandler.h"
#include "DateHandler.h"

using std::vector, std::ios, std::set;

namespace fs = std::filesystem;

#ifndef DIR_TABLE
    #define DIR_TABLE "ScratchQL/FolderTables/"
#endif

typedef enum files_list {ALL, HEADERS_ONLY, TABLES_ONLY} files_list; 

typedef struct EntityProperties{
    string name;
    int idx_col;
    DataType type;
    EntityProperties(string _name="", DataType _type=TYPE_NULL,size_t id_col=0): 
        name(_name), type(_type),idx_col(id_col){};
}EntityProperties;

std::string get_name_filepath(const std::string& name_filepath, bool is_header) {
    std::string suffix = is_header ? "_header.bin" : "_data.bin";
    fs::path filepath = (name_filepath + suffix);
    return filepath.string();
}

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
        case DataType::DATE     : return DateHandler().bin_size();
        case DataType::INT      : return IntHandler().bin_size();
        case DataType::STRING   : return StringHandler().bin_size();
        case DataType::FLOAT    : return FloatHandler().bin_size();
        case DataType::TYPE_NULL: default: break;
    }
    throw std::runtime_error("[ERROR] Fail to figure out which type is");

}


std::set<string> list_tables(files_list filter = ALL)
{
    fs::path path = DIR_TABLE;
    std::set<string> table_list;
    for (const auto & entry : fs::directory_iterator(path)){
        bool condition = false;
        string file_name = entry.path().string();
        switch (filter){
            case files_list::ALL:{ condition = true;}
            case files_list::HEADERS_ONLY:{condition = file_name.find("header.bin");break;}
            case files_list::TABLES_ONLY:{condition = file_name.find("data.bin");break;}
            default: continue;
        }
        if(condition) table_list.insert(file_name);
    }
    return table_list;
}


class FileHandler {
private:
    std::fstream header_file, data_file;
    std::string header_filepath, data_filepath;

    size_t total_elements, offset_row, offset_header, total_entities;
    vector<EntityProperties> entities;
    bool is_tmp;

// Abrir/Fechar um arquivo
    void ptr_file_open(std::fstream &file, const std::string& path, std::ios::openmode mode);
    void open_data(std::ios::openmode mode);
    void open_header(std::ios::openmode mode);

public:
     
    // void display_entities(){std::cout << "Entities:\n";for(int i=0; i < entities.size(); i++)std::cout << "name: " << entities[i].name;std::cout <<"\n";}


    void close();

// Construtor generico que inicializa os caminhos completos dos arquivos e o modo de abertura
    FileHandler(const std::string& filename_base, 
                std::ios::openmode header_mode = ios::binary | ios::out ,
                std::ios::openmode data_mode = ios::binary   | ios::out ,
                bool temporary = false);

// construtor para criação de tabela - deleta se existe algum header
    FileHandler(const string& filename_base, 
                vector<EntityProperties> properties, 
                bool temporary = false);

// Construtor para abertura de tabela(raise error if not existent)
    FileHandler(const string& filename_base, 
                bool temporary = false);


    ~FileHandler();

    void open(bool create = false);    // check if there are any header. If not, create
    void create();  // forced creation, even if exists is deleted
    void destroy(); // delete files: header and data


// Reposicionar o cursor de leitura/escrita no arquivos
    void seek_header(std::ios::seekdir dir, std::streampos pos = 0);
    void seek_data(std::ios::seekdir dir, std::streampos pos = 0);


// indicar posicao de leitura
    std::streampos tell_header();
    std::streampos tell_data();

// Leitura de parametros do header
    void read_header();
    void write_header();
    size_t get_total_elements() const {return total_elements;};
    size_t get_offset_row()     const {return offset_row;};
    size_t get_offset_header()  const {return offset_header;};
    size_t get_total_entities() const {return entities.size();};

    void set_total_elements(int _total_elements){this->total_elements = _total_elements;};
    void set_offset_row(int _offset_row){this->offset_row = _offset_row;};
    void set_offset_header(int _offset_header){this->offset_header = _offset_header;};
    void set_total_entities(int _total_entities){this->total_entities = _total_entities;};
    void set_entities(vector<EntityProperties> entities){this->entities = entities;};



// Leitura header
    const vector<EntityProperties>& get_entities() const {return entities;}
    bool valid_header();

// Leitura  data
    vector<DataInterface*> read_row(size_t row, bool is_RNN = false); 
    size_t bin_fsize(); 
    size_t row_offset();

// Status gerais
    bool eof_header() const;
    bool eof_data() const;
    bool is_temporary() const {return is_tmp;};
    bool valid_insert(vector<DataInterface*> new_value) const;
    string get_header_name() const{return header_filepath;}
    string get_data_name() const{return data_filepath;}


// vinculo com DataInterface
    void write_data(DataInterface* data_ptr);
    void write_data(vector<DataInterface*> data_ptr);
    void read_data(DataInterface* data_ptr);
};


FileHandler::FileHandler(const string& filename_base,
                         ios::openmode header_mode,
                         ios::openmode data_mode,
                         bool temporary)
    :header_filepath(get_name_filepath(filename_base,false)), 
     data_filepath(get_name_filepath(filename_base, true)), is_tmp(temporary),
     total_elements(0), offset_row(0), offset_header(0), total_entities(0)
{
    open();
    write_header();
}

FileHandler::FileHandler(const string& filename_base, 
                        vector<EntityProperties> properties, 
                        bool temporary)
            :header_filepath(DIR_TABLE + filename_base + "_header.bin"), 
             data_filepath(DIR_TABLE + filename_base + "_data.bin"), is_tmp(temporary),
             entities(properties),
             total_elements(0), offset_row(0), offset_header(0), total_entities(0)
{
    create();
    write_header();
}


FileHandler::FileHandler(const string& filename_base, 
                         bool temporary)
            :header_filepath(DIR_TABLE + filename_base + "_header.bin"), 
             data_filepath(DIR_TABLE + filename_base + "_data.bin"), is_tmp(temporary),
             total_elements(0), offset_row(0), offset_header(0), total_entities(0)             
{
    open();
    read_header();

    if(list_tables(HEADERS_ONLY).count(header_filepath) == 0)
        throw std::runtime_error("[ERROR] Header not found !");
    
}

bool FileHandler::valid_header(){
    bool val = false; 
    open_header(ios::out);
    seek_header(ios::end);
    size_t file_size = tell_header();
    size_t min_required = 4 * sizeof(size_t);
    close();
    return min_required <= file_size;
}
size_t FileHandler::row_offset(){
    size_t ans = 0;
    for(auto ptr : entities) ans += data_size(ptr.type);
    return ans;
}

size_t FileHandler::bin_fsize(){
    open_data(ios::in);
    seek_data(ios::end);
    size_t size = tell_data();
    close();
    return  size;
}

vector<DataInterface*> FileHandler::read_row(size_t idx, bool is_RRN)
{
    open_data(ios::in);
    
    size_t offset = idx * (is_RRN ? 1 : row_offset());
    seek_data(ios::beg, offset);
    size_t total_entities = get_total_entities();


    vector<DataInterface*> row(total_entities, nullptr);

    for(int it = 0;it < total_entities; it++)
    {
        row[it] = dt_alloc(entities[it].type);
        // read_data(row[it]);
        row[it]->fread(data_file);
    }
    close();
    return row;
}

void FileHandler::write_data(vector<DataInterface*> data_vector) { // standard is append
    
    if(!valid_insert(data_vector)) return;
    
    open_data(ios::out | ios::app);
    for(auto itr : data_vector) itr->fwrite(data_file);
    total_elements++;
    write_header();
    close();
}

void FileHandler::write_data(DataInterface* data_ptr) {
    open_data(ios::out);
    data_ptr->fwrite(data_file);
    close();
}

void FileHandler::read_data(DataInterface* data_ptr) {
    open_data(std::ios::out);
    data_ptr->fread(data_file);
    close();
}

void FileHandler::ptr_file_open(std::fstream &file, const std::string& path, std::ios::openmode mode){    
    
    // if(file.is_open()) file.close();
    file.open(path,ios::binary | mode);
    
    if (!file.is_open())
        throw std::runtime_error("Falha ao abrir o arquivo: " + path);
    
}
void FileHandler::open_data(std::ios::openmode mode){
    ptr_file_open(data_file, data_filepath,std::ios::binary | mode);
}

void FileHandler::open_header(std::ios::openmode mode){
    ptr_file_open(header_file, header_filepath,std::ios::binary | mode);
}
void FileHandler::destroy(){
    close();
    std::remove(header_filepath.c_str());
    std::remove(data_filepath.c_str());
}

void FileHandler::create(){
    ptr_file_open(header_file, header_filepath, ios::binary | ios::out);
    std::cout << "data\n";
    ptr_file_open(data_file,   data_filepath,   ios::binary | ios::out);
}

void FileHandler::open(bool create_header){
    
    set<string> tables = list_tables(HEADERS_ONLY);
    // 1. if exists, open
    if(tables.count(header_filepath)!= 0){
        ptr_file_open(header_file, header_filepath, ios::binary | ios::in);
        ptr_file_open(data_file,   data_filepath,   ios::binary | ios::in);
        read_header();
        return;
    }
    // 2. if not, create
    if(create_header) create();
}

void FileHandler::close() {
    if (header_file.is_open()) header_file.close();
    if (data_file.is_open())   data_file.close();
}

void FileHandler::seek_header(std::ios::seekdir dir, std::streampos pos) {
    if(!header_file.is_open()){
        throw std::runtime_error("Arquivo de header não está aberto.");
    }

    header_file.seekg(pos, dir);
}

void FileHandler::seek_data(std::ios::seekdir dir, std::streampos pos) {
    // open_data(ios::in);
    if(!data_file.is_open())
        throw std::runtime_error("Arquivo de data não está aberto.");
    data_file.seekg(pos, dir);
}

std::streampos FileHandler::tell_header() {return header_file.tellg();}

std::streampos FileHandler::tell_data() {return data_file.tellg();}

bool FileHandler::eof_header() const {return header_file.eof();}

bool FileHandler::eof_data() const {return data_file.eof();}

// bool FileHandler::valid_insert(vector<DataInterface*> new_row) const{

//     if(new_row.size() != get_total_entities()) return false;

//     for(int i = 0; i < new_row.size(); i++) 
//         if(new_row[i]->read_DataType() != entities[i].type) return false;
//     return true;
// }

bool FileHandler::valid_insert(vector<DataInterface*> row) const{
    
    if(row.size() != get_total_entities()) 
        return false;
    vector<EntityProperties> entity = get_entities();
    for(int i = 0; i < row.size(); i++){
        if(row[i]->read_DataType() != entity[i].type) return false;
    }
    return true;
}



void FileHandler::write_header() {
    // if (!header_file.is_open()) {
    //     throw std::runtime_error("Arquivo de cabeçalho não está aberto.");
    // }
    open_header(ios::out);

    size_t total_elements, offset_row, offset_header, total_entities;

    total_elements = get_total_elements();
    offset_row     = get_offset_row();
    offset_header  = get_offset_header();
    total_entities = get_total_entities();

    // std::cout << "total_elements \t" << total_elements <<"\n";
    // std::cout << "offset_row \t" << offset_row <<"\n";
    // std::cout << "offset_header \t" << offset_header <<"\n";
    // std::cout << "total_entities \t" << total_entities <<"\n\n";

    header_file.write(reinterpret_cast<char*>(&total_elements), sizeof(total_elements));
    header_file.write(reinterpret_cast<char*>(&offset_row), sizeof(offset_row));
    header_file.write(reinterpret_cast<char*>(&offset_header), sizeof(offset_header));
    header_file.write(reinterpret_cast<char*>(&total_entities), sizeof(total_entities));

    DataType tmp_type;
    StringHandler str;

    for(size_t i = 0; i < total_entities; i++){
        tmp_type = entities[i].type;
        str.parseString(entities[i].name);
        str.fwrite(header_file);
        header_file.write(reinterpret_cast<char*>(&tmp_type), sizeof(tmp_type));
    }
    close();
}
void FileHandler::read_header() {
    
    if (!valid_header())
        throw std::runtime_error("[ERROR] Invalid parameters for header, from file - " + get_header_name()); 
    
    open_header(ios::in | ios::out);
    
    seek_header(ios::beg);
    size_t total_elements, offset_row, offset_header, total_entities;
    header_file.read(reinterpret_cast<char*>(&total_elements), sizeof(total_elements));
    header_file.read(reinterpret_cast<char*>(&offset_row), sizeof(offset_row));
    header_file.read(reinterpret_cast<char*>(&offset_header), sizeof(offset_header));
    header_file.read(reinterpret_cast<char*>(&total_entities), sizeof(total_entities));
    
    set_total_elements(total_elements);
    set_offset_row(offset_row);
    set_offset_header(offset_header);
    set_total_entities(total_entities);

    this->entities.clear();
    
    DataType tmp_type;
    StringHandler str;

    for(size_t i = 0; i < total_entities; i++){
        str.fread(header_file);
        header_file.read(reinterpret_cast<char*>(&tmp_type), sizeof(tmp_type));
        entities.push_back(EntityProperties(str.toString(),tmp_type, i));
    }
}


FileHandler::~FileHandler(){
    if(is_temporary()) destroy();
    else close();
}



#endif /*FILEHANDLER_H_*/