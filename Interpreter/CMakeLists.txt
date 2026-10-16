cmake_minimum_required(VERSION 3.22)

set(proj_name Interpreter_lib)

include_directories(include)

set(INTERPRETER_SOURCES
    "src/Lexer.cpp"
    "src/Parser.cpp"
    "src/SchemaFile.cpp"
    "src/Catalog.cpp"
    "src/Executor.cpp"
    "src/Interpreter.cpp"
)

add_library(${proj_name} STATIC ${INTERPRETER_SOURCES})
target_include_directories(${proj_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# The storage layer is header-based but BinPlusTree lives in a .cpp inside
# ScratchQL_lib, so the interpreter links against it.
target_link_libraries(${proj_name} PUBLIC ScratchQL_lib)
target_compile_features(${proj_name} PUBLIC cxx_std_17)

# Interpreter executable (REPL / batch runner).
add_executable(interpreter "main.cpp")
target_link_libraries(interpreter PRIVATE ${proj_name})
set_target_properties(interpreter
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

# Unit tests.
add_executable(interpreter_ut "unit_tests/interpreter_ut.cpp")
target_link_libraries(interpreter_ut PRIVATE ${proj_name})
add_test(NAME interpreter_ut COMMAND interpreter_ut)
