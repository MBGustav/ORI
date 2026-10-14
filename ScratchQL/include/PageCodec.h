#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>

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
};
