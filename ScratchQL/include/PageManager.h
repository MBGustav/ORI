#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

#include "FileManager.h"
#include "Page.h"
#include "PageCodec.h"
#include "baseline_common.h"

class PageManager {
public:
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

bool PageManager::readPage(
    PageID id,
    uint8_t* buffer
) {
    const uint64_t offset = static_cast<uint64_t>(id) * PAGE_SIZE;
    file.read(offset, buffer, PAGE_SIZE);
    return true;
}

bool PageManager::writePage(
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
