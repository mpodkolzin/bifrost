
#pragma once

#include <cstdint>
#include <vector>

enum class RecordType : uint8_t {
    DATA = 1,
    SNAPSHOT = 2,
    HEARTBEAT = 3,
};

class WALRecord {
public:
    RecordType record_type;
    uint64_t timestamp;
    uint64_t sequence_number;
    std::vector<uint8_t> payload;

    WALRecord(RecordType type, const std::vector<uint8_t>& data);

    std::vector<uint8_t> serialize() const;
    static WALRecord deserialize(const std::vector<uint8_t>& buffer);

private:
    static uint64_t current_time_millis();
    static uint32_t calculate_checksum(const std::vector<uint8_t>& data);
    static uint8_t read_u8(const std::vector<uint8_t>& buf, size_t& offset);
    static uint32_t read_u32(const std::vector<uint8_t>& buf, size_t& offset);
    static uint64_t read_u64(const std::vector<uint8_t>& buf, size_t& offset);
};
