#pragma once

#include <string>
#include <vector>
#include "file_handle.h"
#include <unordered_map>



class FileManager {
public:
    FileManager(const std::string& basePath);
    FileHandle openForWrite(uint64_t shardId);
    FileHandle openForRead(uint64_t shardId, uint64_t offset);
    void rotateIfNeeded(uint64_t shardId);
    void deleteOldSegments(uint64_t shardId, uint64_t upToOffset);
    ~FileManager();

private:
    std::string basePath_;
    std::unordered_map<uint64_t, std::vector<FileHandle>> shardFiles_;
};