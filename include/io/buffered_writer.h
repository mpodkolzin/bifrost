#pragma once

#include <string>
#include <wal/wal_record.h>


class BufferedWriter {
public:
    BufferedWriter(int fd, size_t buffer_size = 4096);
    ~BufferedWriter();
    void write(const WALRecord& data);
    void flush();
private:
    int fd_;
    size_t bufferLimit_;
    std::vector<uint8_t> buffer_;
};