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
     * Record pages (NODE_TYPE_RECORD) store table rows. The layout is:
     *   offset 0 : type          (1 byte, NODE_TYPE_RECORD)
     *   offset 1 : record_count  (2 bytes)
     *   offset 3 : reserved      (2 bytes)
     *   offset 5 : records, back to back. Each record is:
     *              uint32 value_count, then value_count values, where each
     *              value is uint8 tag + payload (mirrors RecordStore's codec).
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

    static std::size_t encodedRecordsSize(const std::vector<Record>& records) {
        std::size_t total = page_layout::RECORD_PAGE_HEADER_SIZE;
        for (const Record& record : records) {
            total += encodedRecordSize(record);
        }
        return total;
    }

    /**
     * @brief Encode @p records into a fixed-size record page @p buffer.
     *
     * @throws std::invalid_argument if @p buffer is null.
     * @throws std::length_error if @p size is too small for all @p records.
     */
    static void encodeRecords(
        const std::vector<Record>& records,
        uint8_t* buffer,
        std::size_t size
    ) {
        if (buffer == nullptr) {
            throw std::invalid_argument("PageCodec::encodeRecords: null buffer");
        }

        const std::size_t required = encodedRecordsSize(records);
        if (size < required) {
            throw std::length_error("PageCodec::encodeRecords: buffer too small");
        }

        std::memset(buffer, 0, size);
        buffer[0] = page_layout::NODE_TYPE_RECORD;
        page_layout::write_u16_le(buffer + 1, static_cast<uint16_t>(records.size()));

        std::size_t offset = page_layout::RECORD_PAGE_HEADER_SIZE;
        for (const Record& record : records) {
            page_layout::write_u32_le(
                buffer + offset,
                static_cast<uint32_t>(record.values.size())
            );
            offset += sizeof(uint32_t);

            for (const Value& value : record.values) {
                offset = writeValueAt(buffer, offset, value);
            }
        }
    }

    /**
     * @brief Decode a record page from @p buffer.
     *
     * @throws std::invalid_argument if the page is null or not a record page.
     * @throws std::length_error if the page is truncated.
     */
    static std::vector<Record> decodeRecords(
        const uint8_t* buffer,
        std::size_t size
    ) {
        if (buffer == nullptr) {
            throw std::invalid_argument("PageCodec::decodeRecords: null buffer");
        }
        if (size < page_layout::RECORD_PAGE_HEADER_SIZE) {
            throw std::length_error("PageCodec::decodeRecords: buffer smaller than header");
        }
        if (buffer[0] != page_layout::NODE_TYPE_RECORD) {
            throw std::invalid_argument("PageCodec::decodeRecords: not a record page");
        }

        const uint16_t count = page_layout::read_u16_le(buffer + 1);
        std::size_t offset = page_layout::RECORD_PAGE_HEADER_SIZE;

        std::vector<Record> records;
        records.reserve(count);

        for (uint16_t i = 0; i < count; ++i) {
            if (offset + sizeof(uint32_t) > size) {
                throw std::length_error("PageCodec::decodeRecords: truncated record");
            }
            const uint32_t valueCount = page_layout::read_u32_le(buffer + offset);
            offset += sizeof(uint32_t);

            Record record;
            record.values.reserve(valueCount);
            for (uint32_t j = 0; j < valueCount; ++j) {
                record.values.push_back(readValueAt(buffer, offset, size));
            }
            records.push_back(std::move(record));
        }

        return records;
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
            throw std::length_error("PageCodec::decodeRecords: truncated value");
        }

        const uint8_t tag = buffer[offset++];

        switch (tag) {
            case 0: {
                if (offset + sizeof(int64_t) > size) {
                    throw std::length_error("PageCodec::decodeRecords: truncated int value");
                }
                int64_t number = 0;
                std::memcpy(&number, buffer + offset, sizeof(number));
                offset += sizeof(number);
                return number;
            }
            case 1: {
                if (offset + sizeof(double) > size) {
                    throw std::length_error("PageCodec::decodeRecords: truncated double value");
                }
                double number = 0.0;
                std::memcpy(&number, buffer + offset, sizeof(number));
                offset += sizeof(number);
                return number;
            }
            case 2: {
                if (offset + 1 > size) {
                    throw std::length_error("PageCodec::decodeRecords: truncated bool value");
                }
                const bool flag = buffer[offset++] != 0u;
                return flag;
            }
            case 3: {
                if (offset + sizeof(uint32_t) > size) {
                    throw std::length_error("PageCodec::decodeRecords: truncated string length");
                }
                const uint32_t length = page_layout::read_u32_le(buffer + offset);
                offset += sizeof(uint32_t);
                if (offset + length > size) {
                    throw std::length_error("PageCodec::decodeRecords: truncated string");
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
