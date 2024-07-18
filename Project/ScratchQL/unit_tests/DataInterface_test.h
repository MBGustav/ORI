#include <iostream>
#include <assert.h>
#include "DataInterface.h"
#include "StringHandler.h"
#include "DateHandler.h"
#include "FloatHandler.h"
#include "IntHandler.h"

void testConstructorAndGetValue() {
    IntHandler handler(10);
    assert(handler.get_value() == 10);
    std::cout << "[PASSED] testConstructorAndGetValue" << std::endl;
}

void testSetValue() {
    IntHandler handler(10);
    handler.set_value(20);
    assert(handler.get_value() == 20);
    std::cout << "[PASSED] testSetValue" << std::endl;
}

void testBinSize() {
    IntHandler handler(10);
    assert(handler.bin_size() == sizeof(int));
    std::cout << "[PASSED] testBinSize" << std::endl;
}

void testReadDataType() {
    IntHandler handler(10);
    assert(handler.read_DataType() == INT);
    std::cout << "[PASSED] testReadDataType" << std::endl;
}

void testToString() {
    IntHandler handler(10);
    assert(handler.toString() == "10");
    std::cout << "[PASSED] testToString" << std::endl;
}

void testParseString() {
    IntHandler handler(0);
    std::string data = "30";
    handler.parseString(data);
    assert(handler.get_value() == 30);
    std::cout << "[PASSED] testParseString" << std::endl;
}

void testDataType(){
    IntHandler handler(0);
    std::string data = "45";
    handler.parseString(data);
    assert(handler.read_DataType() == DataType::INT);

    std::cout << "[PASSED] testParseString" << std::endl;
}
void IntHandlerTest()
{
    testConstructorAndGetValue();
    testSetValue();
    testBinSize();
    testReadDataType();
    testToString();
    testParseString();
    testDataType();
}


void FloattestConstructorAndGetValue() {
    FloatHandler handler(10.5f);
    assert(handler.get_value() == 10.5f);
    std::cout << "[PASSED] Float - testConstructorAndGetValue" << std::endl;
}

void FloattestSetValue() {
    FloatHandler handler(10.5f);
    handler.set_value(20.5f);
    assert(handler.get_value() == 20.5f);
    std::cout << "[PASSED] Float - testSetValue" << std::endl;
}

void FloattestBinSize() {
    FloatHandler handler(10.5f);
    assert(handler.bin_size() == sizeof(float));
    std::cout << "[PASSED] Float - testBinSize" << std::endl;
}

void FloattestReadDataType() {
    FloatHandler handler(10.5f);
    assert(handler.read_DataType() == FLOAT);
    std::cout << "[PASSED] Float - testReadDataType" << std::endl;
}

void FloattestToString() {
    FloatHandler handler(10.5f);
    assert(handler.toString() == "10.500000");
    std::cout << "[PASSED] Float - testToString" << std::endl;
}

void FloattestParseString() {
    FloatHandler handler(0.0f);
    std::string data = "30.5";
    handler.parseString(data);
    assert(handler.get_value() == 30.5f);
    std::cout << "[PASSED] Float - testParseString" << std::endl;
}



void FloatHandlerTest()
{
    FloattestConstructorAndGetValue();
    FloattestSetValue();
    FloattestBinSize();
    FloattestReadDataType();
    FloattestToString();
    FloattestParseString();
}


void StringtestConstructorAndGetValue() {
    StringHandler handler("10.5f");
    assert(handler.get_value() == "10.5f");
    std::cout << "[PASSED] String - testConstructorAndGetValue" << std::endl;
}

void StringtestSetValue() {
    StringHandler handler("10.5f");
    string t = "20.5f";
    handler.set_value(t);
    assert(handler.get_value() == "20.5f");
    std::cout << "[PASSED] String - testSetValue" << std::endl;
}

void StringtestBinSize() {
    StringHandler handler("10.5f");
    assert(handler.bin_size() == STR_MAX_SIZE);
    std::cout << "[PASSED] String - testBinSize" << std::endl;
}

void StringtestReadDataType() {
    StringHandler handler("10.5f");
    assert(handler.read_DataType() == DataType::STRING);
    std::cout << "[PASSED] String - testReadDataType" << std::endl;
}

void StringtestToString() {
    StringHandler handler("10.5f");
    assert(handler.toString() == "10.5f");
    std::cout << "[PASSED] String - testToString" << std::endl;
}

void StringtestParseString() {
    StringHandler handler("Texto");
    std::string data = "Texto";
    handler.parseString(data);
    assert(handler.get_value() == "Texto");
    std::cout << "[PASSED] String - testParseString" << std::endl;
}


void StringtestFread() {

    StringHandler handler("Texto");
    fstream file("StringtestFread", std::ios::in | std::ios::out);
    handler.fread(file);
    assert(handler.get_value() == "StringtestFread");
    std::cout << "[PASSED] String - StringtestFread" << std::endl;

}

void StringtestFwrite() {

    StringHandler handler("Texto");

    fstream file("StringtestFwrite", std::ios::in | std::ios::out);
    handler.fwrite(file);
    file.close();

    char t[STR_MAX_SIZE-1];
    char sz;
    string str;

    std::fstream f("StringtestFwrite", std::ios::in | std::ios::out);

    f.read(&t[0], STR_MAX_SIZE-1);
    f.read(reinterpret_cast<char*>(&sz), sizeof(sz));

    str.assign(t,sz);

    assert(t == "Texto" && sz == 5);   

    std::cout << "[PASSED] String - StringtestFwrite" << std::endl;

}

void StringTestFile() {
    // Test data
    string original_data[] = {"Hello, World!", "Outro Test", "C++ rocks!"};
    string file_name = "test_string_handler.bin";

    vector<StringHandler> vec_write;
    // Create a StringHandler object with the test data
    for(auto it : original_data)
        vec_write.push_back(StringHandler(it));
    

    // Write the data to a binary file
    {
        fstream file(file_name, std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for writing");
        }
        for(auto it : vec_write)
            it.fwrite(file);
        
        file.close();
    }

    // Read the data from the binary file
    vector<StringHandler> vec_read;
    
    fstream fread(file_name, std::ios::in | std::ios::binary);
    if (!fread.is_open()) {
        throw std::runtime_error("Failed to open file for reading");
    }
    while(fread.peek() != EOF)
    {
        StringHandler sh_read;
        sh_read.fread(fread);
        vec_read.push_back(sh_read);
    }
    fread.close();
    

    // Verify that the data read is the same as the original data
    assert(vec_read.size() == vec_write.size());
    bool sucess = true;
    int i =0 ;
    for(i =0; i < vec_read.size(); i++){
        if(vec_read[i].get_value() != vec_write[i].get_value()){
            sucess = false;
            break;
        }
    }

    if(sucess) std::cout << "[PASSED] String - TestFile" <<std::endl;
    else std::cout << "[FAILED]  data does not match (" << vec_read[i].get_value() <<" != " << vec_write[i].get_value() <<")\n";
}

void StringHandlerTest()
{
    StringtestConstructorAndGetValue();
    StringtestSetValue();
    StringtestBinSize();
    StringtestReadDataType();
    StringtestToString();
    StringtestParseString();

    StringTestFile();

}

bool DataInterface_test()
{
    IntHandlerTest();  
    FloatHandlerTest();
    StringHandlerTest();
    return true;
}

    // std::cout << test_StringHandler() << std::endl;
    // std::cout << test_DataHandler() << std::endl;

