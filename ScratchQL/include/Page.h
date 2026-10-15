#pragma once


#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

#include "baseline_common.h"

/*
 * Page.h
 *
 * Shared binary layout of a page. It is kept in its own header so that both
 * PageCodec and PageManager can use it without creating a circular include.
 *
 * Layout (little-endian):
 *   offset 0 : type          (1 byte)
 *   offset 1 : key_count     (2 bytes)
 *   offset 3 : child_count   (2 bytes)
 *   offset 5 : keys          (8 bytes each)
 *   following: child refs    (8 bytes each)
 *   remainder: free space
 */
namespace page_layout {
    constexpr uint8_t NODE_TYPE_LEAF = 1;
    constexpr uint8_t NODE_TYPE_INTERNAL = 2;
    constexpr uint8_t NODE_TYPE_RECORD = 3;   // data page holding table rows
    constexpr size_t PAGE_HEADER_SIZE = 5;
    constexpr size_t RECORD_PAGE_HEADER_SIZE = 5;
    constexpr size_t PAGE_KEY_SIZE = sizeof(uint64_t);
    constexpr size_t PAGE_REF_SIZE = sizeof(uint64_t);

    inline uint16_t read_u16_le(const uint8_t* ptr) {
        return static_cast<uint16_t>(
            static_cast<uint16_t>(ptr[0]) |
            (static_cast<uint16_t>(ptr[1]) << 8)
        );
    }

    inline void write_u16_le(uint8_t* ptr, uint16_t value) {
        ptr[0] = static_cast<uint8_t>(value & 0xFFu);
        ptr[1] = static_cast<uint8_t>((value >> 8) & 0xFFu);
    }

    inline uint32_t read_u32_le(const uint8_t* ptr) {
        return static_cast<uint32_t>(ptr[0]) |
               (static_cast<uint32_t>(ptr[1]) << 8) |
               (static_cast<uint32_t>(ptr[2]) << 16) |
               (static_cast<uint32_t>(ptr[3]) << 24);
    }

    inline void write_u32_le(uint8_t* ptr, uint32_t value) {
        ptr[0] = static_cast<uint8_t>(value & 0xFFu);
        ptr[1] = static_cast<uint8_t>((value >> 8) & 0xFFu);
        ptr[2] = static_cast<uint8_t>((value >> 16) & 0xFFu);
        ptr[3] = static_cast<uint8_t>((value >> 24) & 0xFFu);
    }

    inline uint64_t read_u64_le(const uint8_t* ptr) {
        uint64_t value = 0;
        std::memcpy(&value, ptr, sizeof(value));
        return value;
    }

    inline void write_u64_le(uint8_t* ptr, uint64_t value) {
        std::memcpy(ptr, &value, sizeof(value));
    }
}

struct Page {
    uint8_t type = page_layout::NODE_TYPE_LEAF;
    uint16_t key_count = 0;
    uint16_t child_count = 0;
    std::vector<uint64_t> keys;
    std::vector<PageID> children;
};
