#include "wal/wal_record.h"
#include "io/buffered_writer.h"
#include <vector>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>

BufferedWriter::BufferedWriter(std::shared_ptr<FileHandle> file, size_t bufferLimit) : file_(file), bufferLimit_(bufferLimit) {
}

BufferedWriter::~BufferedWriter() {
    flush();
}

void BufferedWriter::write(const WALRecord& record) {
    std::vector<uint8_t> serialized = record.serialize();  // You'll define this
    buffer_.insert(buffer_.end(), serialized.begin(), serialized.end());

    if (buffer_.size() >= bufferLimit_) {
        std::cout << "Flushing buffer" << std::endl;
        flush();
    }
}

void BufferedWriter::flush() {
    file_->write(buffer_.data(), buffer_.size());
    buffer_.clear();
}

