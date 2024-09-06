
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <limits>
#include "DataInterface.h"
#include "SimpleTableHandler.h"
#include "FileHandler.h"

void readTable();

void mostrartabelas();


void changeTable(string basicString);

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
        cout<< "Caso queira escrever uma nova tabela digite(CREATE) 1.\n"
               "caso queira ler sua tabela ou fazer pesquisas nela digite(SELECT) 2.\n"
               "caso queira atualizar a sua tabela (INSERT,UPDATE) 3.\n"
               "caso queira deletar a sua tabela digite(DELETE TABLE) 4.\n"
               "caso queira deletar uma linha da sua tabela digite(DELETE ROW) 5.\n"
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
                cout << "Qual tabela deseja atualizar? ";
                string table_name;
                getline(cin, table_name);
                SimpleTableHandler test(table_name, false);
                test.display();
                cout << "Caso deseje inserir nova linha digite(INSERT) 1.\n"
                        "caso deseje atualizar uma linha digite(UPDATE) 2.\n"
                        "caso deseje sair digite 99: ";
                int op;
                cin >> op;
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
                if(op == 1){
                    vector<DataInterface*> row;
                    auto columns = test.get_entities();
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

                    remove(table_name.c_str());
                }else if(op == 2){
                    changeTable(table_name);
                }else if(op == 99){
                    break;
                }else{
                    cout << "Opção inválida. Saindo.." << endl;
                    break;
                }

                break;
            }
            case 4 : {
                cout << "Qual tabela deseja deletar? ";
                string table_name;
                getline(cin, table_name);
                SimpleTableHandler::delete_table(table_name);
                break;
            }
            case 5 : {
                cout << "Qual tabela deseja deletar? ";
                string table_name;
                getline(cin, table_name);
                SimpleTableHandler test(table_name, false);
                test.display();
                cout << "Digite o indice primário da linha que deseja deletar: ";
                string valor;
                getline(cin, valor);
                test.delete_row(valor);
                cout << endl;
                test.display();
                break;
            }
            case 99 : break;
            default : {
                cout << "Opção inválida" << endl;
                break;
            }

        }

    }

    return 0;
}

void changeTable(string basicString) {
    SimpleTableHandler test(basicString, false);
    test.display();
    cout << "Digite o indice primário da linha que deseja atualizar: ";
    string valor;
    getline(cin, valor);
    vector<size_t> size_ent(test.get_total_entities(), 0);
    int idx = 0;
    size_t total = test.total_items();
    size_t total2print = std::min((size_t)10, total);
    vector<vector<DataInterface *>> vis_table;
    vector<DataInterface *> row;
    vector<DataInterface *> row2;
    auto columns = test.get_entities();
    while (idx < total2print) {
        row = test.read_row(idx++);

        if (row.size() != test.get_total_entities()) break;
        vis_table.push_back(row);

        // Atualiza o tamanho máximo dos dados por coluna
        for (int i = 0; i < test.get_total_entities(); i++) {
            size_ent[i] = std::max(size_ent[i], (row[i]->toString()).size());
        }

        // Procurar pela coluna com o nome especificado
        for (int i = 0; i < test.get_total_entities(); i++) {
            if (valor == row[i]->toString()) {
                // Registro encontrado
                int numero_coluna = 1;
                cout << "Registro encontrado: " << endl;
                for (auto &data : row) {
                    cout << "Valor da Coluna " << numero_coluna << " == " << data->toString() << " ";
                    cout << endl;
                    if(numero_coluna != 1){
                        string input;
                        cout << "Digite o novo valor para " << columns[numero_coluna-1].name << ": ";
                        getline(cin, input); // Use getline to read the entire line
                        row[numero_coluna-1] = dt_alloc(columns[numero_coluna-1].type, input);
                    }
                    numero_coluna++;
                }
                cout << endl;
                cout << "Registro atualizado: " << endl;
                for(auto &data : row){
                    cout << data->toString() << " ";
                }
                cout << endl;
                test.update(idx-1, row);

                break;  // Saia do loop quando encontrar a correspondência
            }
        }

    }

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
        vector<size_t> size_ent(test.get_total_entities(), 0);
        int idx =0;
        //Collect size to print
        size_t total = test.total_items();
        size_t total2print = std::min((size_t)10, total);
        vector<vector<DataInterface *>> vis_table;
        vector<DataInterface *> row;
        while(idx < total2print){
            row = test.read_row(idx++);

            if(row.size()!= test.get_total_entities()) break;
            vis_table.push_back(row);


            for(int i=0; i < test.get_total_entities();i++) {
                size_ent[i] = std::max(size_ent[i], (row[i]->toString()).size());
            }
            for(auto &data : row){
                if(valor == data->toString()){
                    int numero_coluna = 1;
                    cout << "Registro encontrado: " << endl;
                    for (auto &data : row) {
                        cout << "Valor da Coluna " << numero_coluna << " == " << data->toString() << " ";
                        numero_coluna++;
                    }
                    cout << endl;
                    break;
                }
            }
        }
        if (row.empty()) {
            cout << "Nenhum registro encontrado" << endl;
        }
    }else if(tipo == 2){
        string coluna;
        cout << "Digite o nome da Coluna que deseja fazer uma pesquisa: " << endl;
        getline(cin, coluna);
        cout << "Digite o valor que deseja pesquisar: " << endl;
        getline(cin, valor);

        vector<size_t> size_ent(test.get_total_entities(), 0);
        int idx = 0;

// Coletar o tamanho para impressão
        size_t total = test.total_items();
        size_t total2print = std::min((size_t)10, total);
        vector<vector<DataInterface *>> vis_table;
        vector<DataInterface *> row;
        auto columns = test.get_entities();

        while (idx < total2print) {
            row = test.read_row(idx++);

            if (row.size() != test.get_total_entities()) break;
            vis_table.push_back(row);

            // Atualiza o tamanho máximo dos dados por coluna
            for (int i = 0; i < test.get_total_entities(); i++) {
                size_ent[i] = std::max(size_ent[i], (row[i]->toString()).size());
            }

            // Procurar pela coluna com o nome especificado
            for (int i = 0; i < test.get_total_entities(); i++) {
                if (coluna == columns[i].name && valor == row[i]->toString()) {
                    // Registro encontrado
                    int numero_coluna = 1;
                    cout << "Registro encontrado: " << endl;
                    for (auto &data : row) {
                        cout << "Valor da Coluna " << numero_coluna << " == " << data->toString() << " ";
                        numero_coluna++;
                    }
                    cout << endl;
                    break;  // Saia do loop quando encontrar a correspondência
                }
            }
        }
        if (row.empty()) {
            cout << "Nenhum registro encontrado" << endl;
        }
    }else if(tipo == 3){
        return;
    }else{
        cout << "Opção inválida. Saindo.." << endl;
    }
}




