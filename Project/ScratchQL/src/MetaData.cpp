#include "MetaData.h"


bool fileExists(const std::string& filename) {

    std::ifstream file(filename.c_str());
    return file.good();
}

string format_print(data_t val2print)
{   
    DataType tp = val2print.tp;
    switch(tp)
    {
        case STRING:  return val2print.value_str;
        case INT   :  return to_string(val2print.value_int);
        case FLOAT :  return to_string(val2print.value_float);
        case BOOL  :  return (val2print.value_bool ? "True" : "False");
        default: Type_error();
    }
    return "";
}


string GenerateFileName(string FileName = "temp_table")
{
    size_t idx = 1;
    string baseName = string(DIRECTORY_FOR_TABLES) + FileName;
    string output_name = baseName;

    while(fileExists(output_name))    
        output_name = baseName + to_string(idx++);

    return output_name + string(TABLE_EXT);
}
//List all tables existents and still exists
vector<string>ListTables()
{
    vector<string> ans;
    for(const auto &entry : fs::directory_iterator(DIRECTORY_FOR_TABLES))
        ans.push_back(entry.path().filename());
    
    return ans;
}


//create a Metadata from Table already knowing the type
MetaData::MetaData(string tableName, v_entity &Properties):
                    _entity(Properties), _tablename(tableName), _totalRows(0)
{
    write();
}

MetaData::~MetaData(){}

MetaData::MetaData(string TableName): _tablename(TableName)
{
    
    if(!fileExists(get_name(true)))
        File_error();
    
    this->read();

}

v_entity MetaData::get_entities() const {return _entity;}

size_t MetaData::get_numRows() const {return _totalRows;}

size_t MetaData::get_sizeHeader() const 
{
    //posicao para indices
    
    size_t first_info = sizeof(size_t) + // num_rows
                        sizeof(size_t) + // size_header
                        sizeof(size_t) + // row_offset 
                        sizeof(size_t) ; // Total Entities


    v_entity v = get_entities(); 
    size_t items = 0;//sizeof(size_t); // 
    //Total de Entidades:
    for(auto &it : v){
        items += sizeof(DataType) + //type of content
                 sizeof(size_t)   + //size of content 
                 it.Name.size()   ;  //content
    }

    return first_info + items;
}


void MetaData::OpenTable(open_mode OpenMode = READ_ONLY)
{
    
    if(streamTable.is_open()) CloseTable();
    std::ios::openmode FileMode = std::ios::binary;
    switch (OpenMode) {
        case APPEND: {
            FileMode |= std::ios::app | std::ios::out | std::ios::in;
            break;
        } 
        case WRITE_NEW: {
            FileMode |= std::ios::out | std::ios::in;
            break;
        }
        case READ_ONLY: {
            FileMode |= std::ios::in;
            break;
        }
        case WRITE_START: {
            FileMode |= std::ios::out | std::ios::in;
            break;
        }
        default:{ //if not, open table as read
            FileMode |= std::ios::in;
            break;
        }
    }

    // Modo de Abertura - leitura e escrita (TODO: otimizar para cada leitura)
    string fullName = get_name(true);
    // Clear any existing flags
    streamTable.clear();
    
    streamTable.open(fullName.c_str(), FileMode);

    // Verificar se o arquivo foi aberto corretamente
    if (!streamTable.is_open())
        File_error();

    if(OpenMode == READ_FROM_TABLE){
        size_t hdr = get_sizeHeader();
        streamTable.seekg(hdr,ios::beg);

    }
}


void MetaData::CloseTable()
{
    streamTable.close();
}


void MetaData::read()
{
    // openTable();

    char buffer[STR_SIZE];
    size_t  table_name_sz;
    size_t row_offset;
    size_t total_entities;
    size_t size_header;
    size_t num_rows;
    
    OpenTable(READ_ONLY);
    
    //Save the name (and size of the name)
    // this->streamTable.read(reinterpret_cast<char*>(&table_name_sz),  sizeof(table_name_sz));
    // this->streamTable.read(&buffer[0],  table_name_sz);
    
    //The total of elements in Tables
    this->streamTable.read(reinterpret_cast<char*>(&num_rows), sizeof(num_rows));

    //The size of header
    this->streamTable.read(reinterpret_cast<char*>(&size_header), sizeof(size_header));
    
    //The offset of each data row
    this->streamTable.read(reinterpret_cast<char*>(&row_offset), sizeof(row_offset));

    //The Total entities in the file
    this->streamTable.read(reinterpret_cast<char*>(&total_entities), sizeof(total_entities));

    
    //The name of each Entity
    v_entity  v(total_entities);
    for(int i =0; i < v.size(); i++)
    {
        size_t sz;
        DataType tp;
        //write size, type and content

        streamTable.read(reinterpret_cast<char*>(&sz), sizeof(sz));
        streamTable.read(reinterpret_cast<char*>(&tp), sizeof(tp));
        streamTable.read(&buffer[0], sz);

        v[i].Name = string(buffer, sz);
        v[i].type = tp;
    }
    
    set_entities(v);
    set_totalRows(num_rows);
    set_headerOffset(size_header);

    CloseTable();


}


void MetaData::write()
{
    // openTable();

    const string table_name     = this->get_name();
    const size_t  table_name_sz = table_name.size();
    const size_t row_offset     = this->get_rowOffset();
    const size_t total_entities = this->total_entities();
    const size_t size_header    = this->get_sizeHeader();
    const size_t num_rows       = this->get_numRows();
    

    
    
    OpenTable(WRITE_NEW);
    //Save the name (and size of the name)
    // this->streamTable.write(reinterpret_cast< const char*>(&table_name_sz),  sizeof(table_name_sz));
    // this->streamTable.write(&table_name[0],  table_name_sz);
    
    //The total of elements in Tables
    this->streamTable.write(reinterpret_cast<const char*>(&num_rows), sizeof(num_rows));
    
    //The size of header
    this->streamTable.write(reinterpret_cast<const char*>(&size_header), sizeof(size_header));
    

    //The offset of each data row
    this->streamTable.write(reinterpret_cast<const char*>(&row_offset), sizeof(row_offset));

    //The Total entities in the file
    this->streamTable.write(reinterpret_cast<const char*>(&total_entities), sizeof(total_entities));


    //The name of each Entity

    const v_entity  v = get_entities();
    for(int i =0; i < v.size(); i++)
    {
        const size_t sz = v[i].Name.size();
        //write size and content
        streamTable.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
        streamTable.write(reinterpret_cast<const char*>(&(v[i].type)), sizeof(v[i].type));
        streamTable.write(&(v[i].Name[0]), sz);                
    }

    CloseTable();
 }


void MetaData::Delete()
{   
    string fullName = get_name(true);
    bool status = std::remove(fullName.c_str());
    if(!status) Delete_Error();
}




//calculate the position in the table to read the row
size_t MetaData::get_offset_row(size_t idx) const{

    if(idx > total_entities()) bound_error();    
    

    return get_sizeHeader() + idx*get_rowOffset();
};

//position to the end of the table -> must be used to write!
size_t  MetaData::get_offset() const{
    return get_sizeHeader() + total_entities()*get_rowOffset(); 
}

string MetaData::get_name(bool fullName) const
{
    if(fullName) return string(DIRECTORY_FOR_TABLES) + _tablename + string(TABLE_EXT);
    return _tablename;
}

int MetaData::get_rowOffset() const
{
    /*TODO: implementar futuramente uma funcao que usa cada campo para calc de offset*/
    size_t ans = 0;
    for(auto &it: get_entities()){
        // cout << it.type << ", " << it.Name << ", "  << sizeof(it.type) << ", " <<size_of(it.type) <<endl;
        ans += size_of(it.type) + sizeof(it.type);

    }
    
    return ans;
}

void MetaData::update_numRows()
{
    OpenTable(WRITE_START);    
    const size_t num_rows = get_numRows();
    this->streamTable.write(reinterpret_cast<const char*>(&num_rows), sizeof(num_rows));
    
    CloseTable();    
}

bool MetaData::insert(vector<data_t> RowData)
{


    //validacao dos tipos de dados presentes no vetor
    if(!validate_data_type(RowData)) return false;

    OpenTable(APPEND);
    
    for (size_t idx = 0; idx < get_entities().size(); idx++)
    {
        DataType type = RowData[idx].tp;
            // this->streamTable.write(reinterpret_cast<const char*>(&row_offset), sizeof(row_offset));
        streamTable.write(reinterpret_cast<const char*>(&type),sizeof(type));
        // std::cout << format_print(RowData[idx])<< ", ";
        switch (type) {
            case BOOL  : {
                bool val_bool = RowData[idx].value_bool;
                streamTable.write(reinterpret_cast<char*>(&val_bool),sizeof(bool));
                break;
            }
            case INT   : {
                int val_int = RowData[idx].value_int;
                streamTable.write(reinterpret_cast<char*>(&val_int),sizeof(val_int));
                break;
            }
            case FLOAT : {
                float val_float = RowData[idx].value_float;
                streamTable.write(reinterpret_cast<char*>(&val_float),sizeof(val_float));
                break;
            }
            case STRING: {
                // redimensiono a string?? TODO (verificar esse caso)
                /*
                posso guardar o tamanho da str, mantendo lixo nas 
                posicoes nao utilizadas. Insiro um byte que especifica
                o tamanho e depois insiro a string
                */ 
                char *val_char = RowData[idx].value_str;
                streamTable.write(reinterpret_cast<char*>(val_char),sizeof(char)*STR_SIZE);
                break;
            }
            default: Type_error();

        }
    }
    
    set_totalRows(get_numRows() + 1);
    CloseTable();



    return true;
}


bool MetaData::validate_data_type(vector<data_t> RowData)
{
    // compare with the total elements(constraint)
    v_entity vec = this->get_entities();
    const size_t TotalEntities = vec.size();

    if(TotalEntities != RowData.size())
        Size_error();

    // iteracao sobre todos os elementos do vetor
    for(int i =0; i < TotalEntities; i++)
    {
        if(vec[i].type !=RowData[i].tp) return false;
    }

    return true;
}


vector<data_t>MetaData::read_row(size_t idx)
{

    const size_t totalEnt = total_entities();
    const size_t numRows  = get_numRows();
    const size_t offset   = get_rowOffset();  
    //Constraint
    if(numRows < idx)
        bound_error();    

    OpenTable(READ_FROM_TABLE);
    
    streamTable.seekg(idx*get_rowOffset(),std::ios::cur);
    // std::cerr << "Pointers stoped at: " 
    //         << streamTable.tellg() << ", "
    //         << streamTable.tellp() << std::endl;

    vector<data_t>vec(totalEnt);

    for(int i =0 ; i < totalEnt; i++)
    {
        DataType tp;
        streamTable.read(reinterpret_cast<char*>(&tp),sizeof(tp));
        vec[i].tp = tp;
        switch (tp)
        {
            case BOOL  : {
            bool val_bool = vec[i].value_bool;
            streamTable.read(reinterpret_cast<char*>(&val_bool),sizeof(bool));
            break;
        }
        case INT   : {
            int val_int;
            streamTable.read(reinterpret_cast<char*>(&(val_int)),sizeof(val_int));
            vec[i].value_int = val_int;
            break;
        }
        case FLOAT : {
            float val_float;
            streamTable.read(reinterpret_cast<char*>(&val_float),sizeof(val_float));
            vec[i].value_float = val_float;
            break;
        }
        case STRING: {
            // redimensiono a string?? TODO (verificar esse caso)
            /*
            posso guardar o tamanho da str, mantendo lixo nas 
            posicoes nao utilizadas. Insiro um byte que especifica
            o tamanho e depois insiro a string
            */ 
            char *val_char = vec[i].value_str;
            streamTable.read(reinterpret_cast<char*>(val_char),sizeof(char)*STR_SIZE);
            break;
        }
            default:{
                // std::cerr << "[ERROR]: Pointers stoped at: " 
                //           << streamTable.tellg() << ", "
                //           << streamTable.tellp() << std::endl;
                Type_error();
            } 
        }

    }
    
    return vec;
}