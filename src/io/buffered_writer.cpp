#include "wal/wal_record.h"
#include "io/buffered_writer.h"
#include <vector>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>

BufferedWriter::BufferedWriter(std::shared_ptr<FileHandle> file, size_t bufferLimit) : file_(file), bufferLimit_(bufferLimit) {
}

BufferedWriter::~BufferedWriter() {
    flush();
}

void BufferedWriter::write(const WALRecord& record) {
    std::vector<uint8_t> serialized = record.serialize();  // You'll define this
    buffer_.insert(buffer_.end(), serialized.begin(), serialized.end());

    if (buffer_.size() >= bufferLimit_) {
        flush();
    }
}

void BufferedWriter::flush() {
    //size_t totalWritten = 0;
    //while (totalWritten < buffer_.size()) {
    //    ssize_t bytes = ::write(fd_, buffer_.data() + totalWritten, buffer_.size() - totalWritten);
    //    if (bytes < 0) throw std::runtime_error("Write failed");
    //    totalWritten += bytes;
    //}
    //::fsync(fd_);  // optional but ensures durability
    //buffer_.clear();
}

