#include "StringHandler.h"

StringHandler::StringHandler(string input):_data(input){}

StringHandler::StringHandler(fstream &file){fread(file);};

StringHandler::~StringHandler(){}

size_t StringHandler::bin_size(){return static_cast<size_t>(STR_MAX_SIZE);}

DataType StringHandler::read_DataType() const { return DataType::STRING;}

string StringHandler::toString()
{
    if(!_data.data()) std::cout << "NULL access\n";
    return _data;
}

void StringHandler::set_value(string &val){_data = val;}

void StringHandler::parseString(string &data){set_value(data);}

void StringHandler::fwrite(fstream &file){
    
    char tmp[STR_MAX_SIZE] = {0};
    char sz = (char) _data.size();
    tmp[STR_MAX_SIZE-1] = sz; //fill size (cast to char)
    memcpy(tmp, _data.data(), std::min((int) _data.size(),(int) sz));
    file.write(tmp, STR_MAX_SIZE);

}
void StringHandler::fread(fstream &file){
    if (!file.is_open()) {
        throw std::runtime_error("File not open");
    }

    using namespace std;
    char tmp[STR_MAX_SIZE-1];
    file.read(tmp, STR_MAX_SIZE);

    char sz = static_cast<char>(tmp[STR_MAX_SIZE - 1]);
    _data.assign(tmp, sz); // Only assign up to the actual string size
}