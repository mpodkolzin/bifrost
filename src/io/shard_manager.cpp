#include "io/shard_manager.h"
#include "io/file_handle.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <sstream>



ShardManager::ShardManager(const std::string& baseDir): baseDir_(baseDir) {
    std::filesystem::create_directories(baseDir_);
}

std::string ShardManager::makeShardFileName(int shard_id) const {
    std::ostringstream oss;
    oss << baseDir_ << "/shard_" << shard_id << ".wal";
    return oss.str();
}

std::shared_ptr<FileHandle> ShardManager::getHandleForShard(int shard_id) {
        std::lock_guard lock(mutex_);

        auto it = shardFiles_.find(shard_id);
        if (it != shardFiles_.end()) {
            return it->second;
        }

        std::string path = makeShardFileName(shard_id);
        int fd = ::open(path.c_str(), O_RDWR | O_CREAT, 0644);
        if (fd == -1) {
            throw std::runtime_error("Failed to open shard file: " + path);
        }

        auto handle = std::make_shared<FileHandle>(fd);
        shardFiles_[shard_id] = handle;
        return handle;
    return nullptr;
};


std::vector<std::shared_ptr<FileHandle>> ShardManager::listAllHandles() const {
        std::lock_guard lock(mutex_);
        std::vector<std::shared_ptr<FileHandle>> handles;
        for (const auto& [_, handle] : shardFiles_) {
            handles.push_back(handle);
        }
        return handles;
}

