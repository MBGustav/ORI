#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#include "FileManager.h"
#include "Page.h"
#include "PageCodec.h"
#include "baseline_common.h"

class PageManager {
public:
    PageManager() = default;

    explicit PageManager(
        const std::string& path,
        std::ios::openmode mode = std::ios::in | std::ios::out | std::ios::binary
    ) {
        open(path, mode);
    }

    void open(
        const std::string& path,
        std::ios::openmode mode = std::ios::in | std::ios::out | std::ios::binary
    ) {
        file.open(path, mode);
    }

    void close() {
        file.close();
    }

    bool isOpen() const {
        return file.isOpen();
    }

    // Number of whole PAGE_SIZE pages currently stored in the file.
    uint64_t pageCount() {
        const std::streamoff bytes = file.size();
        if (bytes <= 0) {
            return 0;
        }
        return static_cast<uint64_t>(bytes) / PAGE_SIZE;
    }

    inline bool readPage(
        PageID id,
        uint8_t* buffer
    );

    inline bool writePage(
        PageID id,
        const uint8_t* buffer
    );

private:
    FileManager file;
    PageCodec codec;
};

inline bool PageManager::readPage(
    PageID id,
    uint8_t* buffer
) {
    const uint64_t offset = static_cast<uint64_t>(id) * PAGE_SIZE;
    file.read(offset, buffer, PAGE_SIZE);
    return true;
}

inline bool PageManager::writePage(
    PageID id,
    const uint8_t* buffer
) {
    const uint64_t offset = static_cast<uint64_t>(id) * PAGE_SIZE;

    file.write(
        offset,
        buffer,
        PAGE_SIZE
    );
    return true;
}
