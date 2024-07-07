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

void IntHandlerTest()
{
    testConstructorAndGetValue();
    testSetValue();
    testBinSize();
    testReadDataType();
    testToString();
    testParseString();
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

int main() {
    IntHandlerTest();
    FloatHandlerTest();
    // std::cout << test_StringHandler() << std::endl;
    // std::cout << test_DataHandler() << std::endl;



    return 0;
}