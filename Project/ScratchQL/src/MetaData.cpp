#include "MetaData.h"


bool fileExists(const std::string& filename) {

    std::ifstream file(filename.c_str());
    return file.good();
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
    
    size_t items =  STR_SIZE + 
                    4 * sizeof(size_t);

    v_entity v = get_entities();
    //Total de Entidades:
    for(auto &it : v)
        items += it.Name.size();

    return items;
}


void MetaData::OpenTable(bool is_readMode)
{
    
    if(streamTable.is_open()) return;

    // Modo de Abertura - leitura e escrita (TODO: otimizar para cada leitura)
    string fullName = get_name(true);
    
    std::ios_base::openmode FileMode = std::ios::binary;
    
    if (is_readMode)
        FileMode |= std::ios::in; // Modo de leitura
    else
        FileMode |= std::ios::out | std::ios::trunc; // Modo de escrita

    streamTable.open(fullName.c_str(), FileMode);

    // Verificar se o arquivo foi aberto corretamente
    if (!streamTable.is_open())
        File_error();
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
    
    OpenTable(true);
    
    
    //Save the name (and size of the name)
    this->streamTable.read(reinterpret_cast<char*>(&table_name_sz),  sizeof(table_name_sz));
    this->streamTable.read(&buffer[0],  table_name_sz);
    // table_name.resize(table_name_sz);
    this->set_name(string(buffer, table_name_sz));

    //The size of header
    this->streamTable.read(reinterpret_cast<char*>(&size_header), sizeof(size_header));
    
    //The total of elements in Tables
    this->streamTable.read(reinterpret_cast<char*>(&num_rows), sizeof(num_rows));

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
        //write size and content

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
    

    
    
    OpenTable(false);
    //Save the name (and size of the name)
    this->streamTable.write(reinterpret_cast< const char*>(&table_name_sz),  sizeof(table_name_sz));
    this->streamTable.write(&table_name[0],  table_name_sz);
    
    //The size of header
    this->streamTable.write(reinterpret_cast<const char*>(&size_header), sizeof(size_header));
    
    //The total of elements in Tables
    this->streamTable.write(reinterpret_cast<const char*>(&num_rows), sizeof(num_rows));

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
        // cout << v[i].Name << ", ";
        // cout << "sizes: " << sizeof(sz) << ", "<<  sizeof(v[i].type) <<endl;
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
    
    // int ans = 0;
    // for(auto &it: Entity)
    //     // ans += size_of(it.type);
    return total_entities();
}