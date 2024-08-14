# version
cmake_minimum_required(VERSION 3.22)

# Compiler(s)
# set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS  "-O3 -march=native -ftree-vectorize")

# set(CMAKE_CXX_COMPILER clang++)



# Proj Name
project(ScratchQL LANGUAGES C CXX)

# Local to build
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


# enable testing in the project
include(CTest)
enable_testing()

# Include ScratchQL -- cmakelist
add_subdirectory(ScratchQL)
include_directories(ScratchQL/include)


# Add Interpreter /*TODO*/
# add_subdirectory(Interpreter)
# include_directories(Interpreter/include)


# Folder where we store the Tables
include_directories(FolderTables)
add_executable(unit_test "ScratchQL/unit_tests/unit_test.cpp")
set_target_properties(unit_test
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries(unit_test PRIVATE
    ScratchQL_lib
)

add_executable(cli_interface "ScratchQL/test.cpp")
set_target_properties(cli_interface
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries(cli_interface PRIVATE
    ScratchQL_lib
)
         

# add_executable(main main.cpp)
# set_property(TARGET main PROPERTY CXX_STANDARD 17)
# Set binary reference execution
# set_target_properties(main
        # PROPERTIES RUNTIME_OUTPUT_DIRECTORY
        #  "${CMAKE_CURRENT_SOURCE_DIR}")

# target_link_libraries(main PRIVATE
                    # ScratchQL_lib
                    # Interpreter_lib
# )           








