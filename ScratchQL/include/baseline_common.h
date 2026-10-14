#pragma once

#include <string>
#include <cstddef>
#include <vector>
#include <variant>
#include <cstdint>
#include <utility>

#define MAX_COLUMNS 6
#define MAX_RECORDS 10
#define MAX_RECORD_LENGTH 256
enum class DataType {
    Integer,
    Varchar
    /*
    TODO: 
        Date,
        Float,
        Boolean
        ...    
    */
};


// RecordID is a unique identifier for each record in the table.
using RecordID = uint64_t;

// Page identifiers and size are shared by the page storage headers.
using PageID = uint64_t;
constexpr std::size_t PAGE_SIZE = 4096;

// Data types allowed to be stored in table.
using Value = std::variant<
    int64_t,
    double,
    bool,
    std::string
>;


// Page byte definition to the Tree
enum class PageType : uint8_t {
    InternalNode = 1,
    LeafNode = 2,
    Record = 3,
    Free = 4
};

struct Index {
    std::string key;
    RecordID recordId;

    Index() : key(), recordId(0) {}
    Index(std::string keyValue, RecordID idValue = 0)
        : key(std::move(keyValue)), recordId(idValue) {}
};


struct Record 
{
    std::vector<Value> values;
};
