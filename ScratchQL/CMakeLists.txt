cmake_minimum_required(VERSION 3.22)


# Setting a name..
set(proj_name ScratchQL_lib)


# Include ScratchQL files

include_directories(include)
include_directories(unit_tests)


# Integrate all .cpp files
# NOTE: the storage layer is mostly header-only now; only the files that
# actually exist are listed here. (DataInterface/Float/Int/String/Date handler
# sources were removed.)
set(SQL_SOURCES 
"src/FileHandler.cpp"
"src/BinPlusTree.cpp"
)

# Link Files with the project
add_library(${proj_name} STATIC ${SQL_SOURCES})
target_include_directories(${proj_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# Create a TestFile
