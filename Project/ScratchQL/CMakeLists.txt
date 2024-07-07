

# Setting a name..
set(proj_name ScratchQL_lib)


# Include ScratchQL files

include_directories(include)

# Integrate all .cpp files
set(SQL_SOURCES 
"src/DataInterface.cpp"
"src/Date.cpp"
"src/DateHandler.cpp"
"src/FloatHandler.cpp"
"src/IntHandler.cpp"
"src/StringHandler.cpp"
)

# Link Files with the project
add_library(${proj_name} STATIC ${SQL_SOURCES})

# Create a TestFile
