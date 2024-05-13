#include <iostream>
#include <filesystem>

#include "SQLTable.h"

using namespace std;


// bool fileExists(const std::string& filename) {
//     std::ifstream file(string(filename + TABLE_EXT).c_str());
//     return file.good();
// }

string full_path(string local){
    return std::filesystem::absolute(local).string(); 
}

void write()
{
    // cout <<  full_path(argv[0]) << endl;
    // Cria a Tabela e sua configuracao - e nao apague! :D
    


    SQLTable Table("Test", 
    {
        {"NOME",STRING}, 
        {"IDADE",INT},
        {"QUANTIDADE",INT}, 
        {"CLIENTE",BOOL}
    }, false);

    // data_t d[] = 
    // {
    //     {STRING, "Gustavo"},
    //     {INT, 28},
    //     {INT, 100},
    //     {BOOL, 0}
    // };

    // delete Table;


// cout <<"Tipo " <<format_print(d[1]) << endl;
//     vector<data_t> row;

//     for(int i = 0; i < 4; i++)
//     {
//         row.push_back(d[i]);
//     }

    // cout <<( Table.insertRow(row) ? "added" : "not added") << endl;
    // string name(string(DIRECTORY_FOR_TABLES) + "test");
    // cout << (fileExists(name) ? "Yes" : "No")  << endl;
}

void read()
{
    SQLTable Table("Test");
    // Table.showProp();
    Table.showTable();

}

int main(int argc, char **argv)
{

    write();

    read();

    return 0;
}

