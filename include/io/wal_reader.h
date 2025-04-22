#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "wal/wal_record.h"

class WALReader {
public:
    explicit WALReader(const std::string& file_path, size_t chunk_size = 4096);
    bool next(WALRecord& out_record);

private:
    std::ifstream file;
    std::vector<uint8_t> buffer;
    size_t buffer_offset;
    size_t valid_bytes;

    bool refill_buffer();
    bool find_next_magic(size_t& offset);
};
