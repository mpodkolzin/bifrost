#pragma once

#include "file_handle.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <mutex>

class ShardManager {
public:
    std::shared_ptr<FileHandle> getHandleForShard(int shard_id);
    explicit ShardManager(const std::string& base_dir);
    std::vector<std::shared_ptr<FileHandle>> listAllHandles() const;

private:
    std::string baseDir_;
    mutable std::mutex mutex_;
    std::unordered_map<uint64_t, std::shared_ptr<FileHandle>> shardFiles_;
    std::string makeShardFileName(int shard_id) const;
};