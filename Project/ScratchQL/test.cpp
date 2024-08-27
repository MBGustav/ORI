#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <set>
#include <iostream>
#include <memory>
#include <limits>

#include "HeaderHandler.h"
#include "DataInterface.h"
#include "FloatHandler.h"
#include "IntHandler.h"
#include "StringHandler.h"
#include "DateHandler.h"
#include "SimpleTableHandler.h"
#include "FileHandler.h"

void readTable();

using namespace std;

// Function to get user input for table structure
void createTableStructure() {
    cout << "Digite o nome da tabela: ";
    string table_name;
    getline(cin, table_name); // Use getline to read the entire line
    int num_columns;
    cout << "Digite o numero de colunas da tabela: ";
    cin >> num_columns;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

    vector<EntityProperties> columns;

    for (int i = 0; i < num_columns; i++) {
        string column_name;
        int data_type;
        cout << "Digite o nome da coluna " << (i + 1) << ": ";
        getline(cin, column_name); // Use getline to read the entire line
        cout << "Digite o tipo de dado ( 1: FLOAT, 2: STRING, 3: INT, 4: DATE): ";
        cin >> data_type;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

        DataType type = static_cast<DataType>(data_type);
        columns.push_back({column_name, type});
        cout << "Coluna adicionada: " << column_name << " (" << type << ")" << "" << i <<endl;
    }

    // Set up the SimpleTableHandler with the defined columns
    SimpleTableHandler test(table_name, columns, false);

    int num_rows;
    cout << "Digite o numero de linhas a serem adicionadas: ";
    cin >> num_rows;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

    for (size_t i = 0; i < num_rows; ++i) {
        vector<DataInterface*> row;
        for (const auto &column : columns) {
            string input;
            cout << "Digite o valor para " << column.name << ": ";
            getline(cin, input); // Use getline to read the entire line

            // Allocate the appropriate DataInterface based on the data type
            row.push_back(dt_alloc(column.type, input));
        }
        test.write_row(row);

        // Clean up dynamically allocated memory
        for (auto ptr : row) {
            delete ptr;
        }
    }

    test.display();
    std::remove(table_name.c_str());
}

int main() {
    int sup = 0;
    while(sup != 99){
        cout<< "Caso queira escrever uma nova tabela digite 1."
               "\n caso queria ler a sua tabela digite 2(Não implementado)\n."
               "caso queira atualizar a sua tabela digite 3(não implementado)\n."
               "caso queira deletar a sua tabela digite 4\n."
               "caso queira sair digite 99: ";
        cin >> sup;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
        switch (sup){
            case 1 : createTableStructure();
            break;
            case 2 : {
                readTable();
                break;
            }
            case 3 : {
                //SimpleTableHandler test("test", {}, false);
                //test.update();
                break;
            }
            case 4 : {
                cout << "Qual tabela deseja deletar? ";
                string table_name;
                getline(cin, table_name);
                SimpleTableHandler::delete_table(table_name);
                break;
            }
            case 99 : break;
            default : cout << "Opção inválida" << endl;

        }

    }

    return 0;
}

void readTable() {
    cout << "Digite o nome da tabela: ";
    string table_name;
    getline(cin, table_name); // Use getline to read the entire line
    SimpleTableHandler test(table_name, false);
    test.display();
}




