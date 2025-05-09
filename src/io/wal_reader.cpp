#include "io/wal_reader.h"
#include "io/file_handle.h"

#include <fstream>
#include <iostream>

WALReader::WALReader(std::shared_ptr<FileHandle> file, size_t chunk_size)
    : file(file), buffer(chunk_size), buffer_offset(0), valid_bytes(0) {
}

bool WALReader::refill_buffer() {
    // Slide unread bytes to the beginning
    std::cout << "Refilling buffer" << std::endl;
    if (buffer_offset < valid_bytes) {
        size_t remaining = valid_bytes - buffer_offset;
        std::memmove(buffer.data(), buffer.data() + buffer_offset, remaining);
        valid_bytes = remaining;
    } else {
        valid_bytes = 0;
    }
    buffer_offset = 0;

    // Read more from file

    size_t bytes_read = file->read(buffer.data()+valid_bytes, buffer.size() - valid_bytes);
    std::cout << "Bytes read: " << bytes_read << std::endl;
    valid_bytes += bytes_read;

    return bytes_read > 0;
}

bool WALReader::find_next_magic(size_t& offset) {
    std::cout << "Finding next magic" << std::endl;
    const uint32_t MAGIC = 0xDEC0AD42;

    while (offset + 4 <= valid_bytes) {
        uint32_t potential_magic;
        std::memcpy(&potential_magic, buffer.data() + offset, 4);
        if (potential_magic == MAGIC) {
            return true;
        }
        ++offset;  // Scan forward
    }

    return false;
}

bool WALReader::next(WALRecord& out_record) {
    const size_t MIN_RECORD_HEADER = 4 + 4; // Magic + record size

    while (true) {
        std::cout << "Next record" << std::endl;
        if (buffer_offset + MIN_RECORD_HEADER > valid_bytes) {
            if (!refill_buffer()) return false;
        }

        size_t scan_offset = buffer_offset;
        if (!find_next_magic(scan_offset)) {
            if (!refill_buffer()) return false;
            continue;
        }

        // Try to parse from this magic offset
        if (scan_offset + 8 > valid_bytes) {
            if (!refill_buffer()) return false;
            continue;
        }

        uint32_t record_size;
        std::memcpy(&record_size, buffer.data() + scan_offset + 4, 4);

        size_t total_size = 8 + record_size;
        if (scan_offset + total_size > valid_bytes) {
            if (!refill_buffer()) return false;
            continue;
        }

        try {
            std::vector<uint8_t> slice(buffer.begin() + scan_offset, buffer.begin() + scan_offset + total_size);
            out_record = WALRecord::deserialize(slice);
            buffer_offset = scan_offset + total_size;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "WAL parse error: " << e.what() << " at offset " << scan_offset << "\n";
            buffer_offset = scan_offset + 1; // skip forward and try again
        }
    }
}
