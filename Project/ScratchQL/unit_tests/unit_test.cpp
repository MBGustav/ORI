
#include <iostream>
#include "HeaderHandler.h"
#include "HeaderHandler_test.h"
#include "DataInterface_test.h"
#include "SimpleTableHandler.h"

using namespace std;



void test_SimpleTableHandler() {
    // Test file name
    string file_name = "test_table_handler.bin";

    // Create a SimpleTableHandler object
    SimpleTableHandler table_handler(file_name);

    // Create test data rows
    vector<DataInterface*> row1 = {
        dt_alloc(DataType::STRING, "12345678901"), // CPF
        dt_alloc(DataType::STRING, "Alice"),       // NOME
        dt_alloc(DataType::INT, "30"),             // IDADE
        dt_alloc(DataType::STRING, "New York")     // CIDADE
    };

    vector<DataInterface*> row2 = {
        dt_alloc(DataType::STRING, "98765432100"), // CPF
        dt_alloc(DataType::STRING, "Bob"),         // NOME
        dt_alloc(DataType::INT, "25"),             // IDADE
        dt_alloc(DataType::STRING, "Los Angeles")  // CIDADE
    };

    // Write the test data rows to the binary file
    table_handler.write_row(row1);
    table_handler.write_row(row2);

    // Read the rows back from the file
    vector<DataInterface*> read_row1 = table_handler.read_row(0);
    vector<DataInterface*> read_row2 = table_handler.read_row(1);

    // Verify the data read matches the data written
    bool test_passed = true;

    for (int i = 0; i < row1.size(); i++) {
        if (row1[i]->toString() != read_row1[i]->toString()) {
            test_passed = false;
            cout << "Test failed: row1 column " << i << " does not match" << endl;
            cout << "Original: " << row1[i]->toString() << endl;
            cout << "Read: " << read_row1[i]->toString() << endl;
        }
    }

    for (int i = 0; i < row2.size(); i++) {
        if (row2[i]->toString() != read_row2[i]->toString()) {
            test_passed = false;
            cout << "Test failed: row2 column " << i << " does not match" << endl;
            cout << "Original: " << row2[i]->toString() << endl;
            cout << "Read: " << read_row2[i]->toString() << endl;
        }
    }

    if (test_passed) {
        cout << "Test passed: all data matches" << endl;
    }

    // Clean up
    std::remove(file_name.c_str());

    // Clean up dynamically allocated memory
    for (auto ptr : row1) delete ptr;
    for (auto ptr : row2) delete ptr;
    for (auto ptr : read_row1) delete ptr;
    for (auto ptr : read_row2) delete ptr;
}



int main(){

    // create_tab();

    test_SimpleTableHandler();

    // DataInterface_test();
    // Test_HeaderHandler();
}