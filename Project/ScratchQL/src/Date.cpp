 #include "Date.h"

Date::Date(int day, int month, int year): 
        d(day), m(month), y(year) 
{

}
Date::Date(): d(1), m(1), y(1) {}

// Date Date::operator=(const Date& date)
// {
//     set_date(date.get_month(), date.get_day(), date.get_year());
// }


void Date::set_date(int month, int day, int year)
{
    //validate to set ?
    this->m = month;
    this->d = day;
    this->y = year;
}

Date::~Date()
{
}
bool Date::is_bissext(int _ano){
    return (_ano % 400 == 0) || ( (_ano % 4 == 0) && (_ano % 100 != 0) );
}
bool Date::is_valid(int _mes, int _dia, int _ano)
{
    int feb_add = 0;
    if(is_bissext(_ano) && _mes == 2) feb_add = 1;
    bool date_OK = (0 <= _ano) && (0 < _mes && _mes <=12) && (0<= _dia && _dia <= day_month[_mes -1]+feb_add);

    return date_OK;
}

size_t Date::size() const{return sizeof(this->y)+sizeof(this->d) + sizeof(this->m);};  

int Date::get_day() const{return this->d;}
int Date::get_month() const{return this->m;}
int Date::get_year() const{return this->y;}
string Date::toString() const{
    std::ostringstream str;
    str << std::setw(2) << std::setfill('0') << std::to_string(get_month())
        << std::setw(2) << std::setfill('0') << std::to_string(get_day())
        << std::setw(4) << std::setfill('0') << std::to_string(get_year());
        

    return str.str();
}

void Date::parseString(string &data) 
{
    int day, month, year;
    std::istringstream s_data(data);
    s_data >> std::setw(2) >> month
           >> std::setw(2) >> day
           >> std::setw(4) >> year;    

    set_date(month, day, year);
}