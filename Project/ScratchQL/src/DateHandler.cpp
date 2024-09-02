#ifndef _DATAHANDLER_H_
#define _DATAHANDLER_H_

#include "DateHandler.h"
#include <iostream>



DateHandler::DateHandler(string &input)
{
    parseString(input);
}

void DateHandler::NewDate(int day, int month, int year)
{
    m = month;
    d = day;
    y = year;
}
DateHandler::DateHandler(int month, int day, int year)
{
    NewDate(day, month, year);
}
DateHandler::DateHandler(){}

DataType DateHandler::read_DataType() const {return DataType::DATA_DATE;}

DateHandler::DateHandler(DateHandler &date) 
{
    y = date.get_year();
    m = date.get_month();
    d = date.get_day();
}

DateHandler::~DateHandler(){}


string DateHandler::toString(){
    string ans(std::to_string(d) + "/" +
               std::to_string(m) + "/" +
               std::to_string(y));
    return ans;
}

void DateHandler::parseString(std::string &data){
    
    // Encontrar as posições dos delimitadores '/'
    size_t firstSlash = data.find('/');
    size_t secondSlash = data.find('/', firstSlash + 1);

    if (firstSlash == std::string::npos || secondSlash == std::string::npos) {
        std::cerr << "Formato de data inválido" << std::endl;
        return;
    }

    
    std::string dayStr = data.substr(0, firstSlash);
    int day = std::stoi(dayStr);

    std::string monthStr = data.substr(firstSlash + 1, secondSlash - firstSlash - 1);
    int month = std::stoi(monthStr);

    std::string yearStr = data.substr(secondSlash + 1);
    int year = std::stoi(yearStr);

    // Verificar se a data é válida (opcional)
    if ((month < 1 || month > 12 || day < 1 || day > 31)) { //TODO: fix it - is_valid
        std::cerr << "Data inválida" << std::endl;
        return;
    }


    // Definir a nova data
    NewDate(day, month, year);
}

void DateHandler::fwrite(std::fstream &file){
    file.write(reinterpret_cast<const char*>(this), sizeof(*this));
}

void DateHandler::fread(std::fstream &file){
   file.read(reinterpret_cast<char*>(this), sizeof(*this));
}

bool DateHandler::is_bissext(int _ano){
    return (_ano % 400 == 0) || ( (_ano % 4 == 0) && (_ano % 100 != 0) );
}

bool DateHandler::is_valid(int _dia, int _mes, int _ano)
{
    int feb_add = 0;
    if(is_bissext(_ano) && _mes == 2) feb_add = 1;
    bool date_OK = (0 <= _ano) && (0 < _mes && _mes <=12) && (0<= _dia && _dia <= day_month[_mes -1]+feb_add);
    return date_OK;
}


size_t DateHandler::bin_size() {return sizeof(*this);};  


// bool DateHandler::compare_val(DataInterface* left)const{
//     return true;
// }


bool DateHandler::equal(DataInterface* left)const{
    return true;
}

#endif /*_DATAHANDLER_H_*/