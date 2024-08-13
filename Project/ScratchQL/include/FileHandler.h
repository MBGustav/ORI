#ifndef FILEHANDLER_H_
#define FILEHANDLER_H_ 

#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <set>


#include "DataInterface.h"
#include "IntHandler.h"
#include "FloatHandler.h"
#include "StringHandler.h"


using std::vector, std::ios, std::set;

namespace fs = std::filesystem;

#ifndef DIR_TABLE
    #define DIR_TABLE "ScratchQL/FolderTables/"
#endif



std::set<string> list_tables()
{
    // const std::string path = string(DIR_TABLE);
    fs::path path = DIR_TABLE;

    std::set<string> table_list;
    for (const auto & entry : fs::directory_iterator(path)){
        std::cout << entry.path().filename() << std::endl;
        table_list.insert(entry.path().filename().string());
    }

    return table_list;
}


typedef struct EntityProperties{
    string name;
    int idx_col;
    DataType type;
    EntityProperties(string _name="", DataType _type=TYPE_NULL,size_t id_col=0): 
        name(_name), type(_type),idx_col(id_col){};
}EntityProperties;



class FileHandler {
private:
    std::fstream header_file, data_file;
    std::string header_filepath, data_filepath;

    int total_elements, offset_row, offset_header, total_entities;
    vector<EntityProperties> entities;

    bool is_tmp;

public:

// Construtor que inicializa os caminhos completos dos arquivos e o modo de abertura
    FileHandler(const std::string& filename_base, 
                std::ios::openmode header_mode = ios::binary | ios::out ,
                std::ios::openmode data_mode = ios::binary | ios::out ,
                bool temporary = false);

    // construtor para criação de tablea
    FileHandler(const string& filename_base, 
                vector<EntityProperties> properties, 
                bool temporary = false);

    ~FileHandler();


// Abrir/Fechar um arquivo
    void open(std::fstream &file, const std::string& path, std::ios::openmode mode);
    void close();
    void create(); // forced creation, even if already exists
    void destroy(); // delete files


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
    size_t get_total_entities() const {return total_entities;};

    void set_total_elements(int _total_elements){this->total_elements = _total_elements;};
    void set_offset_row(int _offset_row){this->offset_row = _offset_row;};
    void set_offset_header(int _offset_header){this->offset_header = _offset_header;};
    void set_total_entities(int _total_entities){this->total_entities = _total_entities;};
    void set_entities(vector<EntityProperties> entities){this->entities = entities;};

//valores de leitura do header
    const vector<EntityProperties>& get_entities() const {return entities;}

// Status gerais
    bool eof_header() const;
    bool eof_data() const;
    bool is_temporary() const {return is_tmp;};
    bool valid_insert(vector<DataInterface*> new_value);

// vinculo com DataInterface
    void write_data(DataInterface* data_ptr);
    void write_data(vector<DataInterface*> data_ptr);

    void read_data(DataInterface* data_ptr);

};


FileHandler::FileHandler(const string& filename_base,
                         ios::openmode header_mode,
                         ios::openmode data_mode,
                         bool temporary)
    :header_filepath(DIR_TABLE + filename_base + "_header"), data_filepath(DIR_TABLE + filename_base + "_data"), is_tmp(temporary)
{
    // 1. if already exists, open ; 
    set<string> tables = list_tables();
    if(tables.count(header_filepath)!= 0){
        open(header_file, header_filepath, ios::binary | ios::in);
        open(data_file,   data_filepath,   ios::binary | ios::in);
        read_header();
        return;
    }
    // 2. if dont exist, create
    create();
}

FileHandler::FileHandler(const string& filename_base, 
                        vector<EntityProperties> properties, 
                        bool temporary)
            :header_filepath(DIR_TABLE + filename_base + "_header"), 
             data_filepath(DIR_TABLE + filename_base + "_data"), is_tmp(temporary)
                    
{
    create();

}


void FileHandler::write_data(vector<DataInterface*> data_vector) { // standard is append
    if (!data_file.is_open()) 
        throw std::runtime_error("Arquivo de dados não está aberto.");
    
    seek_header(ios::end);

    if(valid_insert(data_vector))
        for(auto itr : data_vector)
            itr->fwrite(data_file);
}

void FileHandler::write_data(DataInterface* data_ptr) {
    if (!data_file.is_open()) 
        throw std::runtime_error("Arquivo de dados não está aberto.");
    
    data_ptr->fwrite(data_file);
}


void FileHandler::read_data(DataInterface* data_ptr) {
    if (!data_file.is_open())
        throw std::runtime_error("Arquivo de dados não está aberto.");

    data_ptr->fread(data_file);
}
void FileHandler::open(std::fstream &file, const std::string& path, std::ios::openmode mode) {
    file.open(path, mode);
    if (!file.is_open())
        throw std::runtime_error("Falha ao abrir o arquivo: " + path);
}

void FileHandler::destroy(){
    close();
    std::remove(header_filepath.c_str());
    std::remove(data_filepath.c_str());
}

void FileHandler::create(){
    open(header_file, header_filepath, ios::binary | ios::out);
    open(data_file,   data_filepath,   ios::binary | ios::out);
}

void FileHandler::close() {
    if (header_file.is_open()) header_file.close();
    if (data_file.is_open())   data_file.close();
}

void FileHandler::seek_header(std::ios::seekdir dir, std::streampos pos) {header_file.seekg(pos, dir);}

void FileHandler::seek_data(std::ios::seekdir dir, std::streampos pos) {data_file.seekg(pos, dir);}

std::streampos FileHandler::tell_header() {return header_file.tellg();}

std::streampos FileHandler::tell_data() {return data_file.tellg();}

bool FileHandler::eof_header() const {return header_file.eof();}

bool FileHandler::eof_data() const {return data_file.eof();}

bool FileHandler::valid_insert(vector<DataInterface*> new_value)
    {
        // constraint
        const size_t t_ent = get_total_entities();
        if(new_value.size() != t_ent) return false;

        vector<EntityProperties> exp = get_entities();
        for(int i = 0; i < t_ent; i ++){
            if(exp[i].type != new_value[i]->read_DataType()) return false;
        }

        return true;
    }

void FileHandler::write_header() {
    if (!header_file.is_open()) {
        throw std::runtime_error("Arquivo de cabeçalho não está aberto.");
    }
    size_t total_elements, offset_row, offset_header, total_entities;

    total_elements = get_total_elements();
    offset_row     = get_offset_row();
    offset_header  = get_offset_header();
    total_entities = get_total_entities();

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
}
void FileHandler::read_header() {
    if (!header_file.is_open()) {
        throw std::runtime_error("Arquivo de cabeçalho não está aberto.");
    }
    
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

    for(auto i : entities){
        std::cout << i.name << "\n";
        std::cout << i.idx_col << "\n";
        std::cout << i.type << "\n";
    }
}


FileHandler::~FileHandler(){
    if(is_temporary()) destroy();
    else close();
}



#endif /*FILEHANDLER_H_*/