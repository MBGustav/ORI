#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>

/*
 * FileManager.h
 * 
 * @brief A class to handle file operations.
 */


class FileManager {
public:
    FileManager() = default;

    explicit FileManager(
        const std::string& path,
        std::ios::openmode mode = std::ios::in | std::ios::out | std::ios::binary
    );

    void open(
        const std::string& path,
        std::ios::openmode mode = std::ios::in | std::ios::out | std::ios::binary
    );

    void close();

    bool isOpen() const;

    std::streamoff size();

    void read(
        uint64_t offset,
        uint8_t* buffer,
        size_t size
    );

    void write(
        uint64_t offset,
        const uint8_t* buffer,
        size_t size
    );

private:
    std::fstream file;
};


inline FileManager::FileManager(
    const std::string& path,
    std::ios::openmode mode
) {
    open(path, mode);
}

inline void FileManager::open(
    const std::string& path,
    std::ios::openmode mode
) {
    if (file.is_open()) {
        file.close();
    }

    file.clear();
    file.open(path, mode);

    if (!file.is_open()) {
        throw std::runtime_error("Falha ao abrir o arquivo: " + path);
    }
}

inline void FileManager::close() {
    if (file.is_open()) {
        file.close();
    }
}

inline bool FileManager::isOpen() const {
    return file.is_open();
}

inline std::streamoff FileManager::size() {
    file.seekg(0, std::ios::end);
    const std::streamoff total = file.tellg();
    file.seekg(0, std::ios::beg);
    file.clear();
    return total;
}

inline void FileManager::read(
    uint64_t offset,
    uint8_t* buffer,
    size_t size
) {
    file.seekg(static_cast<std::streamoff>(offset));
    file.read(reinterpret_cast<char*>(buffer), static_cast<std::streamsize>(size));

    if (!file) {
        throw std::runtime_error("Falha ao ler do arquivo");
    }
}

inline void FileManager::write(
    uint64_t offset,
    const uint8_t* buffer,
    size_t size
) {
    file.seekp(static_cast<std::streamoff>(offset));

    file.write(
        reinterpret_cast<const char*>(buffer),
        static_cast<std::streamsize>(size)
    );

    if (!file) {
        throw std::runtime_error("Falha ao escrever no arquivo");
    }
}
