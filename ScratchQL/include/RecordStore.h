#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "baseline_common.h"
#include "PageCodec.h"
#include "PageManager.h"

/*
 * RecordStore.h
 *
 * The "RecordStore" layer: owns the table's records and persists them through
 * the page storage stack (PageManager -> FileManager), serializing each page
 * with PageCodec's record-page format.
 *
 * Records are appended in order; a RecordID is the 0-based position of a row
 * across all pages. Rows are packed into fixed PAGE_SIZE pages: a row is added
 * to the current page while it fits, otherwise a new page is started.
 *
 * The in-memory std::vector<Record> is a cache rebuilt from the pages on
 * construction, so `at()`/`size()` never touch the file.
 */
class RecordStore {
public:
    explicit RecordStore(std::string filePath)
        : path_(std::move(filePath))
    {
        ensureFileExists();
        pages_.open(path_, std::ios::in | std::ios::out | std::ios::binary);
        load();
    }

    RecordID insert(const std::vector<Value>& values) {
        Record record{values};

        if (PageCodec::encodedRecordSize(record) + page_layout::RECORD_PAGE_HEADER_SIZE
                > PAGE_SIZE) {
            throw std::runtime_error("RecordStore: record does not fit in a page");
        }

        std::vector<Record> candidate = currentPageRecords_;
        candidate.push_back(record);

        if (PageCodec::encodedRecordsSize(candidate) > PAGE_SIZE) {
            ++currentPage_;
            currentPageRecords_.clear();
            candidate.assign(1, record);
        }

        currentPageRecords_ = std::move(candidate);
        records_.push_back(record);
        writeCurrentPage();

        return static_cast<RecordID>(records_.size() - 1);
    }

    std::size_t size() const {
        return records_.size();
    }

    bool empty() const {
        return records_.empty();
    }

    const Record& at(RecordID id) const {
        if (id >= records_.size()) {
            throw std::out_of_range("RecordStore::at: invalid RecordID");
        }
        return records_[static_cast<std::size_t>(id)];
    }

    const std::string& path() const {
        return path_;
    }

    // Number of data pages currently stored on disk.
    uint64_t pageCount() {
        return pages_.pageCount();
    }

private:
    void ensureFileExists() {
        std::ifstream probe(path_, std::ios::binary);
        if (probe.good()) {
            return;
        }

        std::ofstream create(path_, std::ios::binary);
        if (!create) {
            throw std::runtime_error("RecordStore: cannot create " + path_);
        }
    }

    void writeCurrentPage() {
        uint8_t buffer[PAGE_SIZE] = {0};
        PageCodec::encodeRecords(currentPageRecords_, buffer, PAGE_SIZE);
        pages_.writePage(currentPage_, buffer);
    }

    void load() {
        const uint64_t pageTotal = pages_.pageCount();

        std::vector<uint8_t> buffer(PAGE_SIZE, 0);
        for (uint64_t page = 0; page < pageTotal; ++page) {
            if (!pages_.readPage(page, buffer.data())) {
                throw std::runtime_error("RecordStore: failed to read page");
            }

            std::vector<Record> pageRecords =
                PageCodec::decodeRecords(buffer.data(), PAGE_SIZE);

            for (Record& record : pageRecords) {
                records_.push_back(std::move(record));
            }

            // Keep the last page open so new rows keep filling it.
            if (page + 1 == pageTotal) {
                currentPage_ = page;
                currentPageRecords_ = std::move(pageRecords);
            }
        }
    }

    std::string path_;
    PageManager pages_;
    std::vector<Record> records_;

    PageID currentPage_ = 0;
    std::vector<Record> currentPageRecords_;
};
