#include "io/file_manager.h"

FileManager::FileManager(const std::string& baseDirectory)
    : shardManager_(std::make_unique<ShardManager>(baseDirectory)),
      currentShardId_(0) {}

std::shared_ptr<FileHandle> FileManager::getHandleForShard(int shardId) {
    return shardManager_->getHandleForShard(shardId);
}

std::shared_ptr<FileHandle> FileManager::getActiveWriteHandle() {
    return getHandleForShard(currentShardId_);
}

std::vector<std::shared_ptr<FileHandle>> FileManager::listAllHandles() const {
    return shardManager_->listAllHandles();
}

void FileManager::rotateShard() {
    ++currentShardId_;
}
