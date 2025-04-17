#pragma once

#include <memory>
#include <vector>
#include "io/shard_manager.h"
#include "io/file_handle.h"

class FileManager {
public:
    explicit FileManager(const std::string& baseDirectory);

    // Get handle to a specific shard (pass-through to ShardManager)
    std::shared_ptr<FileHandle> getHandleForShard(int shardId);

    // Get the current active shard for writing (e.g., latest)
    std::shared_ptr<FileHandle> getActiveWriteHandle();

    // Get a list of all active/open handles
    std::vector<std::shared_ptr<FileHandle>> listAllHandles() const;

    // Advance to the next shard (for log rotation)
    void rotateShard();

private:
    std::unique_ptr<ShardManager> shardManager_;
    int currentShardId_;
};