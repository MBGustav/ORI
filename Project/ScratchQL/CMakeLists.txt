

# Setting a name..
set(proj_name ScratchQL_lib)


# Include ScratchQL files

include_directories(include)

# Integrate all .cpp files
set(SQL_SOURCES
    "src/MetaData.cpp"
    "src/SQLTable.cpp"
    "include/ErrorHandler.h"
)

# Link Files with the project
add_library(${proj_name} STATIC ${SQL_SOURCES})

