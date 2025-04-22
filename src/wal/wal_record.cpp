#include "wal/wal_record.h"

#include <chrono>
#include <cstdint>
#include <cstring>
#include <vector>


uint64_t WALRecord::current_time_millis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

uint32_t WALRecord::calculate_checksum(const std::vector<uint8_t>& data) {
    uint32_t checksum = 0;
    for (uint8_t byte : data) {
        checksum = (checksum * 31) + byte;
    }
    return checksum;
}

WALRecord::WALRecord(RecordType type, const std::vector<uint8_t>& data)
    : record_type(type), timestamp(current_time_millis()), payload(data) {}

std::vector<uint8_t> WALRecord::serialize() const {
    const uint32_t MAGIC = 0xDEC0AD42;

    uint32_t payload_size = static_cast<uint32_t>(payload.size());
    uint32_t checksum = calculate_checksum(payload);

    // Record size = all fields after magic and record_size
    uint32_t record_size =
        1 +                 // Record type
        8 +                 // Timestamp
        8 +                 // Sequence number
        4 +                 // Payload size
        payload_size +      // Payload
        4;                  // Checksum

    // Total buffer = 4 (magic) + 4 (record_size) + rest
    std::vector<uint8_t> buffer;
    buffer.reserve(4 + 4 + record_size);

    // Helper to append raw bytes
    auto append_bytes = [&](const void* data, size_t size) {
        const uint8_t* ptr = static_cast<const uint8_t*>(data);
        buffer.insert(buffer.end(), ptr, ptr + size);
    };

    // Avoid lambdas by turning this into a manual sequence:
    uint32_t temp_u32;
    uint64_t temp_u64;
    uint8_t temp_u8;

    temp_u32 = MAGIC;
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&temp_u32),
                                  reinterpret_cast<const uint8_t*>(&temp_u32) + sizeof(temp_u32));

    temp_u32 = record_size;
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&temp_u32),
                                  reinterpret_cast<const uint8_t*>(&temp_u32) + sizeof(temp_u32));

    temp_u8 = static_cast<uint8_t>(record_type);
    buffer.push_back(temp_u8);

    temp_u64 = timestamp;
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&temp_u64),
                                  reinterpret_cast<const uint8_t*>(&temp_u64) + sizeof(temp_u64));

    temp_u64 = sequence_number;
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&temp_u64),
                                  reinterpret_cast<const uint8_t*>(&temp_u64) + sizeof(temp_u64));

    temp_u32 = payload_size;
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&temp_u32),
                                  reinterpret_cast<const uint8_t*>(&temp_u32) + sizeof(temp_u32));

    if (!payload.empty()) {
        buffer.insert(buffer.end(), payload.begin(), payload.end());
    }

    temp_u32 = checksum;
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&temp_u32),
                                  reinterpret_cast<const uint8_t*>(&temp_u32) + sizeof(temp_u32));

    return buffer;
}

WALRecord WALRecord::deserialize(const std::vector<uint8_t>& buffer) {
        const uint32_t MAGIC = 0xDEC0AD42;
    size_t offset = 0;

    // Helper read functions
    // Read and validate magic number
    uint32_t magic = read_u32(buffer, offset);
    if (magic != MAGIC) {
        throw std::runtime_error("Invalid WAL record: bad magic number");
    }

    // Read record size (can be used for additional validation)
    uint32_t record_size = read_u32(buffer, offset);

    // Read record type
    RecordType type = static_cast<RecordType>(read_u8(buffer, offset));

    // Read timestamp
    uint64_t timestamp = read_u64(buffer, offset);

    // Read sequence number
    uint64_t seq = read_u64(buffer, offset);

    // Read payload size and validate space
    uint32_t payload_size = read_u32(buffer, offset);
    if (offset + payload_size + 4 > buffer.size()) { // +4 for checksum
        throw std::runtime_error("Invalid WAL record: payload too large");
    }

    // Read payload
    std::vector<uint8_t> payload(buffer.begin() + offset, buffer.begin() + offset + payload_size);
    offset += payload_size;

    // Read checksum and validate
    uint32_t checksum = read_u32(buffer, offset);
    uint32_t expected_checksum = calculate_checksum(payload);
    if (checksum != expected_checksum) {
        throw std::runtime_error("Checksum mismatch in WAL record");
    }

    // Construct the WALRecord
    WALRecord record(type, payload);
    record.timestamp = timestamp;
    record.sequence_number = seq;

    return record;

}


uint8_t WALRecord::read_u8(const std::vector<uint8_t>& buf, size_t& offset) {
    if (offset + 1 > buf.size()) throw std::runtime_error("Buffer too small for uint8");
    return buf[offset++];
}

uint32_t WALRecord::read_u32(const std::vector<uint8_t>& buf, size_t& offset) {
    if (offset + 4 > buf.size()) throw std::runtime_error("Buffer too small for uint32");
    uint32_t value;
    std::memcpy(&value, &buf[offset], 4);
    offset += 4;
    return value;
}

uint64_t WALRecord::read_u64(const std::vector<uint8_t>& buf, size_t& offset) {
    if (offset + 8 > buf.size()) throw std::runtime_error("Buffer too small for uint64");
    uint64_t value;
    std::memcpy(&value, &buf[offset], 8);
    offset += 8;
    return value;
}

