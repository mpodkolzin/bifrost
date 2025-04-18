#pragma once

#include <vector>
#include <unordered_map>
#include <mutex>
#include "io/buffered_writer.h"
#include "io/file_handle.h"


class WriterPool {
public:
    explicit WriterPool();
    BufferedWriter* getWriterForShard(int shard_id, std::shared_ptr<FileHandle> file);

private:
    std::unordered_map<int,std::unique_ptr<BufferedWriter>> writers_;
    std::mutex mutex_;
};