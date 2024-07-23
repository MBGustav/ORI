#include <iostream>
#include "SQLTable.h"
#include <vector>
#include "SimpleTableHandler.h"
#include "DataInterface.h"
#include "FloatHandler.h"
#include "IntHandler.h"
#include "StringHandler.h"
#include "DateHandler.h"
#include "HeaderHandler.h"
#include <string>
#include <map>

using namespace std;

bool dbg_create()
{
    v_entity v = {
        {"ID_NOME", INT },
        {"ID_CLIENTE", INT },
        {"N_CLIENTE", STRING }, 
        {"N_USER", STRING },
        {"CLIENTE_EXISTE", BOOL }, 
        {"NUMERO_INT", INT},
        {"NUMERO_FLOAT", FLOAT}    
    };


    SQLTable Table();

    
}



int main()
{
    int decision = 0;
    while(decision != 9){
        cout << "1 - Create Table\n2 - Insert Row\n3 - Delete Row\n4 - Update Row\n5 - Exit\n";
        cin >> decision;
        switch (decision)
        {
        case 1:
            string name;
            cout << "Which Name?\n";
            cin >> name;
            int num_cols;
            cout << "How many columns?\n";
            cin >> num_cols;
            vector<pair<string, DataType>> cols(num_cols);
            for(int i=0; i<num_cols; i++){
                string col_name;
                cout << "Column Name?\n";
                cin >> col_name;
                int col_type;
                cout << "Column Type?\n1 - INT\n2 - FLOAT\n3 - STRING\n4 - DATE\n";
                cin >> col_type;
                switch (col_type)
                {
                case 1:
                    cols[i] = make_pair(col_name, DataType::INT);
                    break;
                case 2:
                    cols[i] = make_pair(col_name, DataType::FLOAT);
                    break;
                case 3:
                    cols[i] = make_pair(col_name, DataType::STRING);
                    break;
                case 4:
                    cols[i] = make_pair(col_name, DataType::DATE);
                    break;
                default:
                    cout << "Invalid Type\n";
                    break;
                }
            }

            break;
        case 2:
            cout << "In witch table?\n";
            string table_name;
            cin >> table_name;
            cout << "How many columns?\n";
            int num_cols;
            cin >> num_cols;
            vector<data_t> row(num_cols);
            for(int i=0; i<num_cols; i++){
                string col_name;
                cout << "Column Name?\n";
                cin >> col_name;
                int col_type;
                cout << "Column Type?\n1 - INT\n2 - FLOAT\n3 - STRING\n4 - DATE\n";
                cin >> col_type;
                switch (col_type)
                {
                case 1:
                    int value;
                    cout << "Value?\n";
                    cin >> value;
                    row[i] = {DataType::INT, value};
                    break;
                case 2:
                    float value;
                    cout << "Value?\n";
                    cin >> value;
                    row[i] = {DataType::FLOAT, value};
                    break;
                case 3:
                    string value;
                    cout << "Value?\n";
                    cin >> value;
                    row[i] = {DataType::STRING, value};
                    break;
                case 4:
                    string value;
                    cout << "Value?\n";
                    cin >> value;
                    row[i] = {DataType::DATE, value};
                    break;
                default:
                    cout << "Invalid Type\n";
                    break;
                }
            }
            // Table.insertRow(row);
            break;
        case 3:
            cout << "In wicht table?\n";
            cin >> table_name;
            cout << "Which Row?\n";
            int row;
            cin >> row;
            // Table.deleteRow(row);
            break;
        case 4:
            cout << "In Wicht table?\n";
            cin >> table_name;
            cout << "Which Row?\n";
            int row;
            cin >> row;
            // Table.updateRow(row);
            break;
        case 5:
            cout << "Select Row\n";
            break;
        default:
            cout << "Invalid Option\n";
            break;
        }
    }





    return 0;
}
