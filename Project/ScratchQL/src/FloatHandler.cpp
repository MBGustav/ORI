#include "FloatHandler.h"

FloatHandler::FloatHandler(float float_num): _data(float_num){}

FloatHandler::~FloatHandler(){}

float FloatHandler::get_value(){return _data;}

size_t FloatHandler::bin_size(){return sizeof(float);}

DataType FloatHandler::read_DataType() { return DataType::FLOAT;}


void FloatHandler::set_value(float val){this->_data = val;}

string FloatHandler::toString(){ return std::to_string(get_value());}

void FloatHandler::parseString(string &data){set_value(std::stof(data));}

void FloatHandler::fwrite(std::fstream &file){
    file.write(reinterpret_cast<char*>(this), sizeof(*this));
}

void FloatHandler::fread(std::fstream &file) {
    file.read(reinterpret_cast<char*>(this), sizeof(*this));
}

