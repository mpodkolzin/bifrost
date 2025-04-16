#ifndef WAL_RECORD_H
#define WAL_RECORD_H

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
};

#endif // WAL_RECORD_H
