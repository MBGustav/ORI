#include <iostream>

#include <vector>
#include <utility>
#include <string>
#include <map>

#include "SimpleTableHandler.h"
#include "DataInterface.h"
#include "FloatHandler.h"
#include "IntHandler.h"
#include "StringHandler.h"
#include "DateHandler.h"
#include "HeaderHandler.h"
#include "SimpleTableHandler.h"

// using namespace std;



int main()
{
    int decision = 0;
    string table_name;
    string name; 
    int num_cols;
    while(decision != 9){
        std::cout << "1 - Create Table\n2 - Insert Row\n3 - Delete Row\n4 - Update Row\n5 - Exit\n";
        std::cin >> decision;
        switch (decision)
        {
        case 1:{

            string name;
            std::cout << "Which Name?\n";
            std::cin >> name;
            int num_cols;
            std::cout << "How many columns?\n";
            std::cin >> num_cols;
            vector<std::pair<string, DataType>> cols(num_cols);
            for(int i=0; i<num_cols; i++){
                string col_name;
                std::cout << "Column Name?\n";
                std::cin >> col_name;
                int col_type;
                std::cout << "Column Type?\n1 - INT\n2 - FLOAT\n3 - STRING\n4 - DATE\n";
                std::cin >> col_type;
                switch (col_type)
                {
                case 1:{
                    cols[i] = make_pair(col_name, DataType::INT);
                    break;
                }
                case 2:{
                    cols[i] = make_pair(col_name, DataType::FLOAT);
                    break;
                }
                case 3:{
                    cols[i] = make_pair(col_name, DataType::STRING);
                    break;
                }
                case 4:{
                    cols[i] = make_pair(col_name, DataType::DATE);
                    break;
                }
                default:
                    std::cout << "Invalid Type\n";
                    break;
                }
            }
            break;
        }
        case 2:{
            
            std::cout << "In witch table?\n";
            string table_name;
            std::cin >> table_name;
            std::cout << "How many columns?\n";
            int num_cols;
            std::cin >> num_cols;
            vector<DataType*> row(num_cols);
            for(int i=0; i<num_cols; i++){
                string col_name;
                std::cout << "Column Name?\n";
                std::cin >> col_name;
                int col_type;
                std::cout << "Column Type?\n1 - INT\n2 - FLOAT\n3 - STRING\n4 - DATE\n";
                std::cin >> col_type;
                switch (col_type)
                {
                case 1:{
                    int value;
                    std::cout << "Value?\n";
                    std::cin >> value;
                    row[i] = dt_alloc(DataType::INT, to_string(value));
                    break;
                }
                case 2:{
                    float value;
                    std::cout << "Value?\n";
                    std::cin >> value;
                    row[i] = dt_alloc(DataType::FLOAT, to_string(value));
                    break;
                }
                case 3:{
                    string value;
                    std::cout << "Value?\n";
                    std::cin >> value;
                    row[i] = dt_alloc(DataType::STRING, (value));
                    break;
                }
                case 4:{
                    string value;
                    std::cout << "Value?\n";
                    std::cin >> value;
                    row[i] = dt_alloc(DataType::DATE, value);
                    break;
                }
                default:
                    std::cout << "Invalid Type\n";
                    break;
                }
            }
        }
            // Table.insertRow(row);
            break;
        case 3:{

            std::cout << "In wicht table?\n";
            std::cin >> table_name;
            std::cout << "Which Row?\n";
            int row;
            std::cin >> row;
            // Table.deleteRow(row);
            break;
        }
        case 4:{
            std::cout << "In Wicht table?\n";
            std::cin >> table_name;
        }
            std::cout << "Which Row?\n";
            int row;
            std::cin >> row;
            // Table.updateRow(row);
            break;
        case 5:{
            std::cout << "Select Row\n";
            break;
        }
        default:
            std::cout << "Invalid Option\n";
            break;
        }
    }





    return 0;
}
