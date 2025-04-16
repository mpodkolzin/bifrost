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

    size_t header_size = sizeof(uint8_t) 
                        + sizeof(uint64_t)
                        + sizeof(uint32_t)
                        + sizeof(uint32_t);
    std::vector<uint8_t> buffer(header_size + payload.size());

    size_t offset = 0;
    buffer[offset++] = static_cast<uint8_t>(record_type);

    std::memcpy(&buffer[offset], &timestamp, sizeof(timestamp));
    offset += sizeof(timestamp);

    uint32_t checksum = calculate_checksum(payload);
    std::memcpy(&buffer[offset], &checksum, sizeof(checksum));
    offset += sizeof(checksum);

    std::memcpy(&buffer[offset], payload.data(), payload.size());
    return buffer;
}

WALRecord WALRecord::deserialize(const std::vector<uint8_t>& buffer) {
    if (buffer.size() < sizeof(uint8_t) + sizeof(uint64_t) + sizeof(uint32_t) + sizeof(uint32_t)) {
        throw std::runtime_error("Buffer is too small to deserialize WAL record");
    }

    size_t offset = 0;

    uint64_t timestamp;
    uint32_t checksum;
    uint8_t record_type;

    std::memcpy(&record_type, &buffer[offset], sizeof(record_type));
    offset += sizeof(record_type);

    std::memcpy(&timestamp, &buffer[offset], sizeof(timestamp));
    offset += sizeof(timestamp);

    std::memcpy(&checksum, &buffer[offset], sizeof(checksum));
    offset += sizeof(checksum);

    std::vector<uint8_t> payload(buffer.begin() + offset, buffer.end());
    return WALRecord(static_cast<RecordType>(record_type), payload);
}
