#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "wal/wal_record.h"
#include "io/file_handle.h"

class WALReader {
public:
    explicit WALReader(std::shared_ptr<FileHandle> file, size_t chunk_size = 4096);
    bool next(WALRecord& out_record);

private:
    std::shared_ptr<FileHandle> file;
    std::vector<uint8_t> buffer;
    size_t buffer_offset;
    size_t valid_bytes;

    bool refill_buffer();
    bool find_next_magic(size_t& offset);
};
