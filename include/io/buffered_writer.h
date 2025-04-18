#pragma once

#include <string>
#include "wal/wal_record.h"
#include "io/file_handle.h"


class BufferedWriter {
public:
    BufferedWriter(std::shared_ptr<FileHandle> file, size_t buffer_size = 4096);
    ~BufferedWriter();
    void write(const WALRecord& data);
    void flush();
private:
    std::shared_ptr<FileHandle> file_;
    size_t bufferLimit_;
    std::vector<uint8_t> buffer_;
};