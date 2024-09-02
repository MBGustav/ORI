
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <limits>
#include "DataInterface.h"
#include "SimpleTableHandler.h"
#include "FileHandler.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"


void readTable();

void mostrartabelas();


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
        cout<< "Caso queira escrever uma nova tabela digite 1.\n"
               "caso queira ler sua tabela ou fazer pesquisas nela digite 2.\n"
               "caso queira atualizar a sua tabela digite 3(não implementado).\n"
               "caso queira deletar a sua tabela digite 4.\n"
               "caso queira sair digite 99: ";
        cin >> sup;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
        switch (sup){
            case 1 : createTableStructure();
            break;
            case 2 : {
                mostrartabelas();
                readTable();
                break;
            }
            case 3 : {
                //desenvolver função para uptade.
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


void mostrartabelas() {
    cout << "Tabelas disponiveis: " << endl;
    auto list = list_tables(HEADERS_ONLY);
    for (const auto &table : list) {
        cout << table << endl;
    }

}

void readTable() {
    cout << "Digite o nome da tabela: ";
    string table_name;
    getline(cin, table_name); // Use getline to read the entire line
    SimpleTableHandler test(table_name, false);
    test.display();
    //seleciona por indice primario ou secundario
    cout << "Quer pesquisar por indice primario ou secundario? (1: primario, 2: secundario , 3: sair): ";
    int tipo;
    string valor;
    cin >> tipo;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
    if(tipo == 1){
        cout << "Digite o valor do indice primario: \n";
        getline(cin, valor);
        auto row = test.read_pkey(valor);
        if (row.empty()) {
            cout << "Nenhum registro encontrado" << endl;
        } else {
            int numero_coluna = 1;
            cout << "Registro encontrado: " << endl;
            for (const auto &data : row) {
                cout << "Valor da Coluna " << numero_coluna << " == " << data->toString() << " ";
                numero_coluna++;
            }
        }
    }else if(tipo == 2){
        string coluna;
        cout<< "Digite o nome da Coluna que deseja fazer uma pesquisa: "<<endl;
        getline(cin, coluna);
        cout<< "Digite o valor que deseja pesquisar: "<<endl;
        getline(cin, valor);
        vector<vector<DataInterface *>> rows;
        rows = test.read_skey(valor, coluna);
        if (rows.empty()) {
            cout << "Nenhum registro encontrado" << endl;
        } else {
            cout << "Registros encontrados: " << endl;
            int numero_coluna;
            int numero_linha = 1;
            for (const auto &row : rows) {
                cout << "Linha " << numero_linha << ": ";
                numero_linha++;
                numero_coluna = 1;
                for (const auto &data : row) {
                    cout << "Valor da Coluna " << numero_coluna << " == " << data->toString() << " ";
                    numero_coluna++;
                }
            }
        }
    }else if(tipo == 3){
        return;
    }else{
        cout << "Opção inválida. Saindo.." << endl;
    }
}




