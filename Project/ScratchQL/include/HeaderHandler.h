// #ifndef HEADERHANDLER_H_
// #define HEADERHANDLER_H_ 

// #include <iostream> 
// #include <string> 
// #include <cstring> /*memcpy*/
// #include <vector>

// #include <fstream>
// #include <algorithm>
// #include <filesystem>
// #include <mutex>
// #include <shared_mutex>
// #include <syncstream>
// #include <map>

// // #include "ErrorHandler.h"
// #include "DataInterface.h"

// #define TABLE_EXT ".tab"
// #define DIRECTORY_FOR_TABLES "FolderTables/"
// #define TOTAL_DATA_DISPLAY (12)

// using std::fstream, std::mutex,
//       std::vector , std::map;

// //checker for table/header existence
// bool table_exists(const std::string& filename);



// // typedef struct EntityProperties{
// //     string name;
// //     size_t idx_col;
// //     DataType type; 
// //     size_t offset; // ??
// //     EntityProperties(string _name="", DataType _type=TYPE_NULL,size_t id_col=0): 
// //         name(_name), type(_type),idx_col(id_col), offset(0){};
// // }EntityProperties;


// class HeaderHandler{
// private:
//     fstream file_ptr;
//     // mutex &file_mtx_;// std::shared_mutex& fileMutex_; ??? TODO
//     string Filename;
    
//     //melhor seria um array ou um map ? acesso por indice e por nome
//     map<string, EntityProperties> Entities;
//     // vector<EntityProperties> Entities;

//     size_t offset_header;
//     size_t total_elements;
//     size_t offset_row;


// public:
//     HeaderHandler(string &Filename); //Constructor for already existent file
//     HeaderHandler( HeaderHandler& header);
//     ~HeaderHandler();

//     void read_header();
//     void write_header();

//     //setters
//     void set_total_elements(size_t total_elements) {this->total_elements = total_elements;};
//     void set_offset_header(size_t offset_header)   {this->offset_header = offset_header;};
//     void set_offset_row(size_t offset_row)         {this->offset_row = offset_row;};
    
//     //getters
//     size_t get_total_elements() const {return this->total_elements;};
//     size_t get_total_entities() const {return Entities.size();};
//     size_t get_offset_header() const  {return this->offset_header;};
//     size_t get_offset_row() const     {return this->offset_row;};

//     vector<EntityProperties> get_EntityList();

//     EntityProperties *get_EntityProperties(size_t idx);
//     EntityProperties *get_EntityProperties(string name_entity); 

//     //just for dbg, showing data from header
//     void display_prop();
    
// };

// #endif /*HEADERHANDLER_H_*/