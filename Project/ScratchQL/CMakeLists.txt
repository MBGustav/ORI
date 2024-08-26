cmake_minimum_required(VERSION 3.22)


# Setting a name..
set(proj_name ScratchQL_lib)


# Include ScratchQL files

include_directories(include)

# Integrate all .cpp files
set(SQL_SOURCES 
"src/DataInterface.cpp"
"src/DateHandler.cpp"
"src/FloatHandler.cpp"
"src/IntHandler.cpp"
"src/StringHandler.cpp"
"src/FileHandler.cpp"
)

# Link Files with the project
add_library(${proj_name} STATIC ${SQL_SOURCES})

# Create a TestFile
