#include <iostream>
#include <string>
#include <vector>

#include "SimpleTableHandler.h"
#include "DataInterface.h"
#include "FloatHandler.h"
#include "StringHandler.h"
#include "IntHandler.h"

using namespace std;


const std::vector<string> vec_name = {"Gwen Holman", "Marnie Huffman", "Phoenix Ibarra", "Youssef Powers", "Luc Carey", "Abdirahman Salas", "Kristian Cole", "Faye Hamilton", "Maia Landry", "Richie Mcconnell", "Alannah O'Doherty", "Gianluca Pena", "Virgil Harrington", "Casper Warner", "Mari Herman", "Taylor Rocha", "Stephanie Chan", "Hope Walters", "Ali Edwards", "Samir Leon", "Keane Cotton", "Abubakar Bush", "Gabriela Sloan", "Jayden Miles", "Alexandre Whitehead", "Solomon Moyer", "Ella Irwin", "Lottie Hebert", "Ralph Mcgowan", "Christina Walsh", "Gladys Turner", "Polly Decker", "Nina George", "Maddie Mcdaniel", "Neha Espinoza", "Lilli Bennett", "Denise Johns", "Mateo Klein", "Christopher Hudson", "Sulaiman Russo", "Travis Hunt", "Angela Beard", "Simeon Ramos", "Jeremy Davidson", "Alexandros Leblanc", "Keisha Beck", "Lola Palmer", "Arman Clark", "Brett Bolton", "Nancy Lozano", "Wayne Montoya", "Sallie Chaney", "Emily Fuller", "Malakai Singh", "Luna Serrano", "Zack Fisher", "Mohsin Wyatt", "Lincoln Owens", "Zaina Rivers", "Kasey Jacobson", "Henrietta Rivera", "Mathilda Nichols", "Zayn Terrell", "Luqman Sullivan", "Lois Farrell", "Cole Church", "Alexandra Bartlett", "Lily-Mae Cunningham", "Cecil Cohen", "Abbie Murphy", "Jakob Stein", "Aled Haas", "Josie Simmons", "Wendy Haines", "Melisa Dixon", "Tori Hoffman", "Lily Wright", "Saoirse Knowles", "Liyana Stanton", "Aliyah Gibson", "Lyla Santana", "Kira Mcmillan", "Muhammad Kramer", "Diana Carlson", "Adriana O'Reilly", "Tina Abbott", "Ricardo Roberson", "Eleri Bell", "Kobe Moses", "Zane Dickerson", "Jaya Mcknight", "Darragh Wilson", "Izabella Mcneil", "Mark Melton", "Betty Mayer", "Kenneth Casey", "Martha Ball", "Martin Doherty", "Caitlin Case", "Meredith Barr"};
const std::vector<string> vec_country = {"Hong Kong","Ethiopia","Sweden","Belize","American Samoa","Dominica","Guatemala","Togo","Ghana","Solomon Islands","Turkmenistan","Bhutan","Democratic Republic of Congo","Tuvalu","Wallis and Futuna","Cook Islands","Eritrea","New Zealand","Israel","Mauritania"};

void test_read_file_shandler() {
    // Test file name
    string file_name = "test_table_handler.tab";

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
            cout << "[FAILED] row2 " << i << " does not match" << endl;
            cout << "Result: " << row2[i]->toString() <<" != "
                 << read_row2[i]->toString() << endl;
        }
    }

    if (test_passed) {
        cout << "[PASSED]: Reading and Writing File with success" << endl;
    }

    // Clean up
    std::remove(file_name.c_str());

    // Clean up dynamically allocated memory
    for (auto ptr : row1) delete ptr;
    for (auto ptr : row2) delete ptr;
    for (auto ptr : read_row1) delete ptr;
    for (auto ptr : read_row2) delete ptr;
}


void populateTable(SimpleTableHandler &Table, size_t size)
{

    std::vector<DataInterface*> row(4);
    row[0] = dt_alloc(DataType::STRING); // CPF
    row[1] = dt_alloc(DataType::STRING); // NOME
    row[2] = dt_alloc(DataType::INT); // IDADE
    row[3] = dt_alloc(DataType::STRING); // CIDADE


    for(int i=0; i < std::min(vec_name.size(),size); i++)
    {   
        string cpf = to_string(i);
        string age = to_string(rand()%100);
        string country = vec_country[i%20];
        string name = vec_name[i];

        row[0]->parseString(cpf);       // CPF
        row[1]->parseString(name);      // NOME
        row[2]->parseString(age);       // IDADE
        row[3]->parseString(country);   // CIDADE
        
        Table.write_row(row);
    }

    for(auto ptr : row) delete ptr;

}

void test_readPrevFile(){
    // Test file name
    string file_name = "test_table.tab";

    std::remove(file_name.c_str());

    // Create a SimpleTableHandler object
    SimpleTableHandler table_handler(file_name);
    populateTable(table_handler, 5);


    // Create another table and import file
    SimpleTableHandler table(file_name);
    table.read_file();


    // Check the last key included  (pkey)
    if(table.valid_pkey("4")) std::cout << "[PASSED] Primary key checking ok.\n";
    else std::cout << "[ERROR] Did not find a single key.\n";

    std::remove(file_name.c_str());

}

void using_pkey_validation(){
    
    string file_name = "test_table_handler.tab";
    SimpleTableHandler T(file_name);

    populateTable(T, 10);
    // T.display();

    T.read_file();

    if(T.valid_pkey("4")) std::cout <<"[PASSED] valid pkey\n";
    else std::cout <<"[ERROR] did not find\n";
    // vector<DataInterface*>row = T.read_pkey("4");
    // for(auto itr: row) std::cout << itr->toString() <<" | ";

    std::remove(file_name.c_str());
}

void query_skey_test()
{
    size_t row_offset = 100;
    string file = "pkey_test.bin";

    SimpleTableHandler T(file);
    populateTable(T, 200);
    T.read_file();

    T.display();

    vector<vector<DataInterface*>> list_query = T.read_skey("Sweden","CIDADE");
    


    SimpleTableHandler NewTable(list_query, "NewTable.tab", false);

    NewTable.display();

    std::remove(file.c_str());
}

void query_skey_upper_test()
{
    size_t row_offset = 100;
    string file = "pkey_test.bin";

    SimpleTableHandler T(file);
    populateTable(T, 200);
    T.read_file();

    vector<vector<DataInterface*>> list_query = T.read_skey_greater("50","IDADE");
    
    SimpleTableHandler NewTable(list_query, "NewTable_upper.tab", false);

    NewTable.display();

    std::remove(file.c_str());
}

void query_pkey_test()
{

    size_t row_offset = 100;
    string file = "pkey_test.bin";

    SimpleTableHandler T(file);
    populateTable(T, 100);
    T.read_file();

    vector<DataInterface*> row, exp_row(4); 
    exp_row[0] = dt_alloc(DataType::STRING); // CPF
    exp_row[1] = dt_alloc(DataType::STRING); // NOME
    exp_row[2] = dt_alloc(DataType::INT);    // IDADE
    exp_row[3] = dt_alloc(DataType::STRING); // CIDADE

    row = T.read_pkey("3");


    std::fstream f(file.c_str(), std::ios::binary | std::ios::in);
    f.seekg(3*row_offset, std::ios::beg);
    for(auto itr:exp_row) itr->fread(f);

    

    for(int i = 0; i < row.size(); i++){
        if(exp_row[i]->toString() != row[i]->toString()){
            throw std::runtime_error("[ERROR] query pkey test Failed, value differs\n");
        }
    }

    f.close();
    std::remove(file.c_str());
    std::cout << "[PASSED] query pkey ok\n";
}

void SimpleHandler_tst(){

    using_pkey_validation();
    test_read_file_shandler();
    test_readPrevFile();

    query_pkey_test();

    query_skey_test();
    query_skey_upper_test();
}