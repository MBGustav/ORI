// ScratchQL interpreter entry point.
//
//   interpreter                 interactive REPL (tables in ScratchQL/FolderTables)
//   interpreter --dir <path>    use <path> as the table directory
//   interpreter -f <file>       run the statements in <file> and exit
//
// Statements are terminated by ';'. Type "exit" or "quit" (or Ctrl-D) to leave.

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Interpreter.h"
#include "Result.h"
#include "SqlError.h"

namespace {

const char* kDefaultTableDir = "ScratchQL/FolderTables";

void printError(const std::exception& error) {
    std::cout << "Error: " << error.what();
    const auto* sqlError = dynamic_cast<const SqlError*>(&error);
    if (sqlError != nullptr && sqlError->line() != 0) {
        std::cout << " (line " << sqlError->line()
                  << ", column " << sqlError->column() << ")";
    }
    std::cout << "\n";
}

void runScript(Interpreter& interpreter, const std::string& sql) {
    for (const Result& result : interpreter.execute(sql)) {
        std::cout << result.toString() << "\n";
    }
}

int runFile(Interpreter& interpreter, const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Cannot open script: " << path << "\n";
        return 1;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    try {
        runScript(interpreter, buffer.str());
    } catch (const std::exception& error) {
        printError(error);
        return 1;
    }
    return 0;
}

int runRepl(Interpreter& interpreter) {
    std::string line;
    while (true) {
        std::cout << "ScratchQL> " << std::flush;
        if (!std::getline(std::cin, line)) {
            std::cout << "\n";
            break;
        }

        if (line == "exit" || line == "quit") {
            break;
        }
        if (line.empty()) {
            continue;
        }

        try {
            runScript(interpreter, line);
        } catch (const std::exception& error) {
            printError(error);
        }
    }
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    std::string tableDir = kDefaultTableDir;
    std::string scriptPath;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--dir" && i + 1 < argc) {
            tableDir = argv[++i];
        } else if (arg == "-f" && i + 1 < argc) {
            scriptPath = argv[++i];
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
            return 1;
        }
    }

    try {
        Interpreter interpreter(tableDir);
        if (!scriptPath.empty()) {
            return runFile(interpreter, scriptPath);
        }
        return runRepl(interpreter);
    } catch (const std::exception& error) {
        printError(error);
        return 1;
    }
}
