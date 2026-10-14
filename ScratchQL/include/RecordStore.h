#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "baseline_common.h"

/*
 * RecordStore.h
 *
 * The "RecordStore" layer: owns the table's records and mirrors them to a
 * binary file. Each record is a row of Values; RecordID is its 0-based
 * position, so records are referenced by index.
 *
 * On-disk format (little-endian, sequential):
 *   uint32  valueCount
 *   value   valueCount times, where each value is
 *           uint8 tag (0=int64, 1=double, 2=bool, 3=string)
 *           payload (int64/double/bool, or uint32 length + bytes for string)
 */
class RecordStore {
public:
    explicit RecordStore(std::string filePath)
        : path_(std::move(filePath)) {
        load();
    }

    RecordID insert(const std::vector<Value>& values) {
        Record record{values};
        append(record);
        records_.push_back(std::move(record));
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

private:
    static void writeValue(std::ostream& out, const Value& value) {
        const uint8_t tag = static_cast<uint8_t>(value.index());
        out.write(reinterpret_cast<const char*>(&tag), sizeof(tag));

        switch (tag) {
            case 0: {
                const int64_t number = std::get<int64_t>(value);
                out.write(reinterpret_cast<const char*>(&number), sizeof(number));
                break;
            }
            case 1: {
                const double number = std::get<double>(value);
                out.write(reinterpret_cast<const char*>(&number), sizeof(number));
                break;
            }
            case 2: {
                const bool flag = std::get<bool>(value);
                out.write(reinterpret_cast<const char*>(&flag), sizeof(flag));
                break;
            }
            case 3: {
                const std::string& text = std::get<std::string>(value);
                const uint32_t length = static_cast<uint32_t>(text.size());
                out.write(reinterpret_cast<const char*>(&length), sizeof(length));
                out.write(text.data(), static_cast<std::streamsize>(length));
                break;
            }
            default:
                throw std::runtime_error("RecordStore: unsupported value type");
        }
    }

    static Value readValue(std::istream& in) {
        uint8_t tag = 0;
        in.read(reinterpret_cast<char*>(&tag), sizeof(tag));

        switch (tag) {
            case 0: {
                int64_t number = 0;
                in.read(reinterpret_cast<char*>(&number), sizeof(number));
                return number;
            }
            case 1: {
                double number = 0.0;
                in.read(reinterpret_cast<char*>(&number), sizeof(number));
                return number;
            }
            case 2: {
                bool flag = false;
                in.read(reinterpret_cast<char*>(&flag), sizeof(flag));
                return flag;
            }
            case 3: {
                uint32_t length = 0;
                in.read(reinterpret_cast<char*>(&length), sizeof(length));
                std::string text(length, '\0');
                if (length > 0) {
                    in.read(&text[0], static_cast<std::streamsize>(length));
                }
                return text;
            }
            default:
                throw std::runtime_error("RecordStore: corrupt record file");
        }
    }

    void append(const Record& record) {
        std::ofstream out(path_, std::ios::binary | std::ios::app);
        if (!out) {
            throw std::runtime_error("RecordStore: cannot open " + path_);
        }

        const uint32_t count = static_cast<uint32_t>(record.values.size());
        out.write(reinterpret_cast<const char*>(&count), sizeof(count));

        for (const Value& value : record.values) {
            writeValue(out, value);
        }
    }

    void load() {
        std::ifstream in(path_, std::ios::binary);
        if (!in) {
            return;
        }

        while (in.peek() != std::char_traits<char>::eof()) {
            uint32_t count = 0;
            in.read(reinterpret_cast<char*>(&count), sizeof(count));
            if (!in) {
                break;
            }

            Record record;
            record.values.reserve(count);

            for (uint32_t i = 0; i < count; ++i) {
                record.values.push_back(readValue(in));
            }

            records_.push_back(std::move(record));
        }
    }

    std::string path_;
    std::vector<Record> records_;
};
