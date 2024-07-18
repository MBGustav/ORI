#include <iostream>
#include <fstream>
#include <mutex>
#include <vector>
#include <filesystem>
#include "HeaderHandler.h"
#include "DataInterface.h"


void createTestFile(const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary | std::ios::out);
    size_t total_elements = 3;
    size_t offset_row = 100;
    size_t offset_header = 200;
    size_t total_entities = 2;

    outFile.write(reinterpret_cast<char*>(&total_elements), sizeof(total_elements));
    outFile.write(reinterpret_cast<char*>(&offset_row), sizeof(offset_row));
    outFile.write(reinterpret_cast<char*>(&offset_header), sizeof(offset_header));
    outFile.write(reinterpret_cast<char*>(&total_entities), sizeof(total_entities));

    DataType dataTypes[] = {INT, STRING};
    outFile.write(reinterpret_cast<char*>(dataTypes), sizeof(dataTypes));

    std::string name1 = "ID";
    std::string name2 = "Name";
    size_t name1_size = name1.size();
    size_t name2_size = name2.size();

    outFile.write(reinterpret_cast<char*>(&name1_size), sizeof(name1_size));
    outFile.write(name1.c_str(), name1.size());
    
    outFile.write(reinterpret_cast<char*>(&name2_size), sizeof(name2_size));
    outFile.write(name2.c_str(), name2.size());
    
    outFile.close();
}

void deleteTestFile(const std::string& filename) {
    std::filesystem::remove(filename);
}

bool testReadHeader() {
    std::mutex testMutex;
    std::string testFilename = "test_table.tab";

    createTestFile(testFilename);
    HeaderHandler headerHandler(testFilename);
    
    bool success = true;
    success &= (headerHandler.get_total_elements() == 3);
    success &= (headerHandler.get_offset_row() == 100);
    success &= (headerHandler.get_offset_header() == 200);
    success &= (headerHandler.get_total_entities() == 2);

    auto entities = headerHandler.get_EntityList();
    success &= (entities.size() == 2);

    success &= (entities[0].name == "ID");
    success &= (entities[0].type == INT);

    success &= (entities[1].name == "Name");
    success &= (entities[1].type == STRING);

    deleteTestFile(testFilename);

    return success;
}

bool testWriteHeader() {
    std::mutex testMutex;
    std::string testFilename = "test_table.tab";

    createTestFile(testFilename);
    
    HeaderHandler WriteHeader(testFilename);

    // Modify header information
    WriteHeader.set_total_elements(5);
    WriteHeader.set_offset_row(300);
    WriteHeader.set_offset_header(400);
    
    WriteHeader.write_header();
    

    // Read back the file to verify changes
    HeaderHandler newHeaderHandler(testFilename);

    bool success = true;
    success &= (newHeaderHandler.get_total_elements() == 5);
    success &= (newHeaderHandler.get_offset_row() == 300);
    success &= (newHeaderHandler.get_offset_header() == 400);

    auto entities = newHeaderHandler.get_EntityList();
    success &= (entities.size() == 2);

    success &= (entities[0].name == "ID");
    success &= (entities[0].type == INT);

    success &= (entities[1].name == "Name");
    success &= (entities[1].type == STRING);

    // deleteTestFile(testFilename);

    return success;
}


bool Test_HeaderHandler(){

    bool success = testReadHeader();

    std::cout << (success  ? "[PASSED]" : "[ERROR ]")  << " test ReadHeader()" << std::endl;

    success &= testWriteHeader(); 
        
    std::cout << (success  ? "[PASSED]" : "[ERROR ]")  << " test WriteHeader()" << std::endl;


    return success;
}