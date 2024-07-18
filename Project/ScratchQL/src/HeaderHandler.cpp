#include "HeaderHandler.h"


bool table_exists(const std::string& filename) {
    std::ifstream fileStream(filename.c_str());
    return fileStream.good();
}



void HeaderHandler::write_header()
{
    file_ptr.open(Filename.c_str(), std::ios_base::binary | std::ios_base::out);
    size_t int_ptr=0, total_entities=0;
    size_t size_name;

    EntityProperties aux_entityProp;

    if(!file_ptr.is_open())
        throw std::runtime_error("[ERROR] File not opened, check if file really exists.");


    //read total elem.
    int_ptr = get_total_elements();
    file_ptr.write(reinterpret_cast<char*>(&int_ptr), sizeof(int_ptr));

    //read offset elem
    int_ptr = get_offset_row();
    file_ptr.write(reinterpret_cast<char*>(&int_ptr), sizeof(int_ptr));

    //read offset header
    int_ptr = get_offset_header();
    file_ptr.write(reinterpret_cast<char*>(&int_ptr), sizeof(int_ptr));

    //read total_ent
    total_entities = get_total_entities();
    file_ptr.write(reinterpret_cast<char*>(&total_entities), sizeof(total_entities));
    
    // iterate over all entitties types
    EntityProperties *datatype_arr = new EntityProperties[total_entities];
    
    
    size_t itr =0;
    for(auto it = Entities.begin(); it != Entities.end(); ++it){
        datatype_arr[itr++] = it->second;
    }
    
    for(size_t it =0; it < total_entities; it ++ ){
        file_ptr.write(reinterpret_cast<char*>(&(datatype_arr[it].type)), sizeof(DataType));
    }

    //iterate over all entities names
    for(itr = 0; itr < total_entities; itr++){
        string name_ent(datatype_arr[itr].name);
        size_name = name_ent.size();
        
        file_ptr.write(reinterpret_cast<char*>(&size_name), sizeof(size_name));
        file_ptr.write(name_ent.c_str(), size_name);
    }

    delete[]  datatype_arr;

    file_ptr.close();
}


void HeaderHandler::read_header()
{
    file_ptr.open(Filename.c_str(), std::ios_base::binary | std::ios_base::in);
    size_t int_ptr=0, total_entities=0;
    size_t size_name;
    EntityProperties aux_entityProp;

    if(!file_ptr.is_open())
        throw std::runtime_error("[ERROR] File not opened, check if file really exists.");


    //read total elem.
    file_ptr.read(reinterpret_cast<char*>(&int_ptr), sizeof(int_ptr));
    set_total_elements(int_ptr);

    //read offset elem
    file_ptr.read(reinterpret_cast<char*>(&int_ptr), sizeof(int_ptr));
    set_offset_row(int_ptr);

    //read offset header
    file_ptr.read(reinterpret_cast<char*>(&int_ptr), sizeof(int_ptr));
    set_offset_header(int_ptr);

    //read total_ent
    file_ptr.read(reinterpret_cast<char*>(&total_entities), sizeof(total_entities));
    
    
    // iterate over all entitties types
    DataType *datatype_arr = new DataType[total_entities];
    file_ptr.read(reinterpret_cast<char*>(&datatype_arr[0]), total_entities * sizeof(DataType));
    
    //iterate over all entities names
    for(size_t itr = 0; itr < total_entities; itr++){
        
        file_ptr.read(reinterpret_cast<char*>(&size_name), sizeof(size_name));
        char *tmp = new char[size_name+1]; //to load the class
        file_ptr.read(&tmp[0], size_name);
        tmp[size_name] = '\0';
        Entities[tmp] = EntityProperties(tmp, datatype_arr[itr]);
        delete[] tmp;
    }

    delete []  datatype_arr;
    file_ptr.close();
}


vector<EntityProperties> HeaderHandler::get_EntityList()
{
    const size_t total_entities = get_total_entities();
    vector<EntityProperties> ent_arr;

    if(total_entities == 0) return ent_arr;

    size_t itr;

    for(auto it=Entities.begin(); itr < total_entities && it != Entities.end(); it++){
        ent_arr.push_back(it->second);
        itr++;
    }

    return ent_arr;
}

HeaderHandler::HeaderHandler(string &Filename): 
              Filename(Filename)
{
    if(table_exists(Filename)){
        read_header();
    }
}

HeaderHandler::HeaderHandler( HeaderHandler& header)
{  
    
    set_offset_header(header.get_offset_header());
    set_offset_row(header.get_offset_row());
    set_total_elements(header.get_total_elements());

}

HeaderHandler::~HeaderHandler(){}

EntityProperties *HeaderHandler::get_EntityProperties(size_t idx)
{
    EntityProperties *copy_of = new EntityProperties;
    copy_of->name = ""; 
    copy_of->type = TYPE_NULL;
    //constraint
    if(idx >= Entities.size()) return copy_of;
    map<string, EntityProperties>::iterator *it;
    int i =0;
    for(*it = Entities.begin(); *it != Entities.end() && i<idx; it++)
        if(i == idx) std::memcpy(copy_of, it, sizeof(EntityProperties));
    
    return copy_of;
}

EntityProperties *HeaderHandler::get_EntityProperties(string name_entity)
{
    EntityProperties *copy_of = new EntityProperties;

    copy_of->name = ""; 
    copy_of->type = TYPE_NULL;

    map<string, EntityProperties>::iterator it = Entities.find(name_entity);

    if(it != Entities.end())
        std::memcpy(copy_of, &it, sizeof(EntityProperties));

    return copy_of;
}

void HeaderHandler::display_prop()
{
    
    std::cout << "\n.::Header Properties::.\n";

    std::cout << " - Nro. rows :     " << get_total_elements() << std::endl;
    std::cout << " - Offset_row:     " << get_offset_row() << std::endl;
    std::cout << " - Offset_header:  " << get_offset_header() << std::endl;
    std::cout << " - Total_entities: " << get_total_entities() << std::endl;


    vector<EntityProperties>  ent = get_EntityList();
    std::cout << "----Entities----\n";
    for(int i =0; i < ent.size(); i++){
    std::cout << "{Name: " << ent[i].name
              << ", Offset: " << ent[i].offset 
              << ", type: " << ent[i].type<<"}" <<std::endl;
    }




}
