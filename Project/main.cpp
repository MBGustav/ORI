#include <iostream>
#include <filesystem>
#include <vector>
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
        {"NOME_CLIENTE",STRING},
        {"IDADE",INT},
        {"QUANTIDADE",INT}, 
        {"ID_CLIENTE",INT}
    }, false);

    data_t d[] = 
    {
        {STRING, "Gustavo"},
        {STRING, "UFScar."},
        {INT, 25},
        {INT, 100},
        {INT, 127}
    };

    std::vector<data_t> row;
    for(auto &it :d)
    {
        row.push_back(it);
    }

    cout <<( Table.insertRow(row) ? "added" : "not added") << endl;
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
    vector<string> str_list = ListTables();
    
    write();

    // read();
    return 0;
}

