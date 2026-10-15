#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>

#include <string>
#include <vector>

#include "Page.h"

/**
 * @brief Classe para codificação e decodificação de páginas.
 *
 * Serializes a Page to/from its fixed binary representation (see Page.h).
 * All operations validate the destination/source buffer size and throw on
 * mismatch instead of silently producing a corrupted page.
 */
class PageCodec {
public:
    /**
     * @brief Number of bytes needed to store @p page.
     */
    static std::size_t encodedSize(const Page& page) {
        return page_layout::PAGE_HEADER_SIZE
            + page.keys.size() * page_layout::PAGE_KEY_SIZE
            + page.children.size() * page_layout::PAGE_REF_SIZE;
    }

    /**
     * @brief Encode @p page into @p buffer.
     *
     * @throws std::invalid_argument if @p buffer is null.
     * @throws std::length_error if @p size is too small for the page.
     */
    void encode(const Page& page, uint8_t* buffer, std::size_t size) const {
        if (buffer == nullptr) {
            throw std::invalid_argument("PageCodec::encode: buffer is null");
        }

        if (page.keys.size() > std::numeric_limits<uint16_t>::max() ||
            page.children.size() > std::numeric_limits<uint16_t>::max()) {
            throw std::length_error("PageCodec::encode: too many keys/children");
        }

        const std::size_t required = encodedSize(page);
        if (size < required) {
            throw std::length_error("PageCodec::encode: buffer too small");
        }

        std::memset(buffer, 0, size);

        buffer[0] = page.type;
        page_layout::write_u16_le(buffer + 1, static_cast<uint16_t>(page.keys.size()));
        page_layout::write_u16_le(buffer + 3, static_cast<uint16_t>(page.children.size()));

        std::size_t offset = page_layout::PAGE_HEADER_SIZE;

        for (const uint64_t key : page.keys) {
            page_layout::write_u64_le(buffer + offset, key);
            offset += page_layout::PAGE_KEY_SIZE;
        }

        for (const PageID child : page.children) {
            page_layout::write_u64_le(buffer + offset, static_cast<uint64_t>(child));
            offset += page_layout::PAGE_REF_SIZE;
        }
    }

    /*
     * Leaf pages of the record B+Tree store the rows themselves. The layout is:
     *   offset 0  : type       (1 byte, NODE_TYPE_LEAF)
     *   offset 1  : key_count  (2 bytes)
     *   offset 3  : next_page  (8 bytes, PageID of the next leaf or 0)
     *   offset 11 : entries, one per row:
     *               uint64 key (RecordID) + uint32 value_count + values,
     *               where each value is uint8 tag + payload.
     */

    static std::size_t encodedValueSize(const Value& value) {
        switch (value.index()) {
            case 0: return 1 + sizeof(int64_t);
            case 1: return 1 + sizeof(double);
            case 2: return 1 + sizeof(uint8_t);
            case 3: return 1 + sizeof(uint32_t) + std::get<std::string>(value).size();
            default:
                throw std::runtime_error("PageCodec: unsupported value type");
        }
    }

    static std::size_t encodedRecordSize(const Record& record) {
        std::size_t total = sizeof(uint32_t);
        for (const Value& value : record.values) {
            total += encodedValueSize(value);
        }
        return total;
    }

    static std::size_t encodedLeafEntrySize(const Record& record) {
        return sizeof(uint64_t) + encodedRecordSize(record);
    }

    static std::size_t encodedLeafSize(
        const std::vector<RecordID>& keys,
        const std::vector<Record>& records
    ) {
        (void)keys;
        std::size_t total = page_layout::LEAF_PAGE_HEADER_SIZE;
        for (const Record& record : records) {
            total += encodedLeafEntrySize(record);
        }
        return total;
    }

    /**
     * @brief Encode leaf entries into a fixed-size leaf page @p buffer.
     *
     * @throws std::invalid_argument if @p buffer is null.
     * @throws std::length_error if @p size is too small for all entries.
     */
    static void encodeLeaf(
        const std::vector<RecordID>& keys,
        const std::vector<Record>& records,
        PageID next,
        uint8_t* buffer,
        std::size_t size
    ) {
        if (buffer == nullptr) {
            throw std::invalid_argument("PageCodec::encodeLeaf: null buffer");
        }

        const std::size_t required = encodedLeafSize(keys, records);
        if (size < required) {
            throw std::length_error("PageCodec::encodeLeaf: buffer too small");
        }

        std::memset(buffer, 0, size);
        buffer[0] = page_layout::NODE_TYPE_LEAF;
        page_layout::write_u16_le(buffer + 1, static_cast<uint16_t>(records.size()));
        page_layout::write_u64_le(buffer + 3, static_cast<uint64_t>(next));

        std::size_t offset = page_layout::LEAF_PAGE_HEADER_SIZE;
        for (std::size_t i = 0; i < records.size(); ++i) {
            page_layout::write_u64_le(buffer + offset, static_cast<uint64_t>(keys[i]));
            offset += sizeof(uint64_t);

            page_layout::write_u32_le(
                buffer + offset,
                static_cast<uint32_t>(records[i].values.size())
            );
            offset += sizeof(uint32_t);

            for (const Value& value : records[i].values) {
                offset = writeValueAt(buffer, offset, value);
            }
        }
    }

    /**
     * @brief Decode a leaf page from @p buffer.
     *
     * @throws std::invalid_argument if the page is null or not a leaf page.
     * @throws std::length_error if the page is truncated.
     */
    static void decodeLeaf(
        const uint8_t* buffer,
        std::size_t size,
        std::vector<RecordID>& keys,
        std::vector<Record>& records,
        PageID& next
    ) {
        if (buffer == nullptr) {
            throw std::invalid_argument("PageCodec::decodeLeaf: null buffer");
        }
        if (size < page_layout::LEAF_PAGE_HEADER_SIZE) {
            throw std::length_error("PageCodec::decodeLeaf: buffer smaller than header");
        }
        if (buffer[0] != page_layout::NODE_TYPE_LEAF) {
            throw std::invalid_argument("PageCodec::decodeLeaf: not a leaf page");
        }

        const uint16_t count = page_layout::read_u16_le(buffer + 1);
        next = static_cast<PageID>(page_layout::read_u64_le(buffer + 3));

        std::size_t offset = page_layout::LEAF_PAGE_HEADER_SIZE;

        keys.clear();
        records.clear();
        keys.reserve(count);
        records.reserve(count);

        for (uint16_t i = 0; i < count; ++i) {
            if (offset + sizeof(uint64_t) + sizeof(uint32_t) > size) {
                throw std::length_error("PageCodec::decodeLeaf: truncated entry");
            }

            const uint64_t key = page_layout::read_u64_le(buffer + offset);
            offset += sizeof(uint64_t);

            const uint32_t valueCount = page_layout::read_u32_le(buffer + offset);
            offset += sizeof(uint32_t);

            Record record;
            record.values.reserve(valueCount);
            for (uint32_t j = 0; j < valueCount; ++j) {
                record.values.push_back(readValueAt(buffer, offset, size));
            }

            keys.push_back(static_cast<RecordID>(key));
            records.push_back(std::move(record));
        }
    }

    /**
     * @brief Decode a Page from @p buffer.
     *
     * @throws std::invalid_argument if @p buffer is null.
     * @throws std::length_error if @p size is smaller than the header or the
     *         declared key/child counts would read past the buffer.
     */
    Page decode(const uint8_t* buffer, std::size_t size) const {
        if (buffer == nullptr) {
            throw std::invalid_argument("PageCodec::decode: buffer is null");
        }

        if (size < page_layout::PAGE_HEADER_SIZE) {
            throw std::length_error("PageCodec::decode: buffer smaller than header");
        }

        Page page;
        page.type = buffer[0];
        page.key_count = page_layout::read_u16_le(buffer + 1);
        page.child_count = page_layout::read_u16_le(buffer + 3);

        const std::size_t required = page_layout::PAGE_HEADER_SIZE
            + static_cast<std::size_t>(page.key_count) * page_layout::PAGE_KEY_SIZE
            + static_cast<std::size_t>(page.child_count) * page_layout::PAGE_REF_SIZE;

        if (size < required) {
            throw std::length_error("PageCodec::decode: truncated page");
        }

        std::size_t offset = page_layout::PAGE_HEADER_SIZE;

        page.keys.reserve(page.key_count);
        for (uint16_t i = 0; i < page.key_count; ++i) {
            page.keys.push_back(page_layout::read_u64_le(buffer + offset));
            offset += page_layout::PAGE_KEY_SIZE;
        }

        page.children.reserve(page.child_count);
        for (uint16_t i = 0; i < page.child_count; ++i) {
            page.children.push_back(
                static_cast<PageID>(page_layout::read_u64_le(buffer + offset))
            );
            offset += page_layout::PAGE_REF_SIZE;
        }

        return page;
    }

private:
    static std::size_t writeValueAt(
        uint8_t* buffer,
        std::size_t offset,
        const Value& value
    ) {
        const uint8_t tag = static_cast<uint8_t>(value.index());
        buffer[offset++] = tag;

        switch (tag) {
            case 0: {
                const int64_t number = std::get<int64_t>(value);
                std::memcpy(buffer + offset, &number, sizeof(number));
                offset += sizeof(number);
                break;
            }
            case 1: {
                const double number = std::get<double>(value);
                std::memcpy(buffer + offset, &number, sizeof(number));
                offset += sizeof(number);
                break;
            }
            case 2: {
                buffer[offset++] = std::get<bool>(value) ? 1u : 0u;
                break;
            }
            case 3: {
                const std::string& text = std::get<std::string>(value);
                page_layout::write_u32_le(buffer + offset, static_cast<uint32_t>(text.size()));
                offset += sizeof(uint32_t);
                if (!text.empty()) {
                    std::memcpy(buffer + offset, text.data(), text.size());
                    offset += text.size();
                }
                break;
            }
            default:
                throw std::runtime_error("PageCodec: unsupported value type");
        }

        return offset;
    }

    static Value readValueAt(
        const uint8_t* buffer,
        std::size_t& offset,
        std::size_t size
    ) {
        if (offset + 1 > size) {
            throw std::length_error("PageCodec::decodeLeaf: truncated value");
        }

        const uint8_t tag = buffer[offset++];

        switch (tag) {
            case 0: {
                if (offset + sizeof(int64_t) > size) {
                    throw std::length_error("PageCodec::decodeLeaf: truncated int value");
                }
                int64_t number = 0;
                std::memcpy(&number, buffer + offset, sizeof(number));
                offset += sizeof(number);
                return number;
            }
            case 1: {
                if (offset + sizeof(double) > size) {
                    throw std::length_error("PageCodec::decodeLeaf: truncated double value");
                }
                double number = 0.0;
                std::memcpy(&number, buffer + offset, sizeof(number));
                offset += sizeof(number);
                return number;
            }
            case 2: {
                if (offset + 1 > size) {
                    throw std::length_error("PageCodec::decodeLeaf: truncated bool value");
                }
                const bool flag = buffer[offset++] != 0u;
                return flag;
            }
            case 3: {
                if (offset + sizeof(uint32_t) > size) {
                    throw std::length_error("PageCodec::decodeLeaf: truncated string length");
                }
                const uint32_t length = page_layout::read_u32_le(buffer + offset);
                offset += sizeof(uint32_t);
                if (offset + length > size) {
                    throw std::length_error("PageCodec::decodeLeaf: truncated string");
                }
                std::string text(
                    reinterpret_cast<const char*>(buffer + offset),
                    length
                );
                offset += length;
                return text;
            }
            default:
                throw std::runtime_error("PageCodec: corrupt record page");
        }
    }
};
