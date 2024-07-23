cmake_minimum_required(VERSION 3.22)


# Setting a name..
set(proj_name ScratchQL_lib)


# Include ScratchQL files

include_directories(include)

# Integrate all .cpp files
set(SQL_SOURCES 
"src/DateHandler.cpp"
"src/FloatHandler.cpp"
"src/IntHandler.cpp"
"src/StringHandler.cpp"
"src/HeaderHandler.cpp"
)

# Link Files with the project
add_library(${proj_name} STATIC ${SQL_SOURCES})

# A unit test to check all over the functions.
add_executable(unit_test "unit_tests/unit_test.cpp")
set_target_properties(unit_test
        PROPERTIES RUNTIME_OUTPUT_DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}")
        target_link_libraries(unit_test PRIVATE
        ScratchQL_lib
)