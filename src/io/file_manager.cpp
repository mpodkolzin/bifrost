#include "io/file_manager.h" // Assume this includes FileHandle and other necessary headers
#include "io/file_handle.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include <iostream>

namespace utils {

// Helper method to get the prefix for a shard's files
std::string getShardPrefix(uint64_t shardId) {
    std::stringstream ss;
    ss << "shard_" << std::setw(10) << std::setfill('0') << shardId;
    return ss.str();
}


// Helper method to generate a filename for a shard segment
std::string generateFilename(uint64_t shardId, uint64_t segmentNum, const std::string &basePath) {
    std::stringstream ss;
    ss << basePath << "/" << utils::getShardPrefix(shardId) << "_" << std::setw(6) << std::setfill('0') << segmentNum << ".dat";
    return ss.str();
}

// Helper method to get the next segment number for a shard
uint64_t getNextSegmentNumber(uint64_t shardId, const std::string &basePath) {
    uint64_t maxSegment = 0;
    std::string shardPrefix = utils::getShardPrefix(shardId);
    
    for (const auto& entry : std::filesystem::directory_iterator(basePath)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.find(shardPrefix) == 0) {
                // Extract the segment number from the filename
                size_t underscorePos = filename.find_last_of('_');
                size_t dotPos = filename.find_last_of('.');
                if (underscorePos != std::string::npos && dotPos != std::string::npos) {
                    std::string segmentStr = filename.substr(underscorePos + 1, dotPos - underscorePos - 1);
                    try {
                        uint64_t segment = std::stoull(segmentStr);
                        maxSegment = std::max(maxSegment, segment);
                    } catch (...) {
                        // Ignore parse errors
                    }
                }
            }
        }
    }
    
    return maxSegment + 1;
}

// Helper method to generate a default filename (latest segment)
std::string generateFilename(uint64_t shardId, const std::string &basePath) {
    return utils::generateFilename(shardId, utils::getNextSegmentNumber(shardId, basePath) - 1, basePath);
}


}

// Constructor initializes the base path for all shard files
FileManager::FileManager(const std::string& basePath) : basePath_(basePath) {
    // Ensure the base directory exists
    if (!std::filesystem::exists(basePath_)) {
        std::filesystem::create_directories(basePath_);
    }
}

// Open a file for writing to the specified shard
FileHandle FileManager::openForWrite(uint64_t shardId) {
    // Generate the filename for the current segment of this shard
    std::string filename = utils::generateFilename(shardId, basePath_);
    
    // Open the file with write permissions, create if it doesn't exist
    int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        // Handle error
        std::cerr << "Failed to open file for writing: " << strerror(errno) << std::endl;
        return FileHandle(-1);
    }
    
    // Create a FileHandle and store it in the shardFiles_ map if not already present
    FileHandle handle(fd);
    if (shardFiles_.find(shardId) == shardFiles_.end()) {
        shardFiles_[shardId] = std::vector<FileHandle>();
    }
    
    // Store the handle if not already in the vector
    bool handleExists = false;
    for (const auto& existingHandle : shardFiles_[shardId]) {
        // This is a simple check - might need a better way to compare FileHandles
        if (existingHandle.isValid()) {
            handleExists = true;
            break;
        }
    }
    
    if (!handleExists) {
        shardFiles_[shardId].push_back(handle);
    }
    
    return handle;
}

// Open a file for reading from the specified shard at the given offset
FileHandle FileManager::openForRead(uint64_t shardId, uint64_t offset) {
    // Generate the filename for the segment of this shard
    std::string filename = utils::generateFilename(shardId, basePath_);
    
    // Open the file with read permissions
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd < 0) {
        // Handle error
        std::cerr << "Failed to open file for reading: " << strerror(errno) << std::endl;
        return FileHandle(-1);
    }
    
    FileHandle handle(fd);
    
    // Seek to the specified offset
    if (handle.seek(offset, SEEK_SET) < 0) {
        std::cerr << "Failed to seek to offset " << offset << ": " << strerror(errno) << std::endl;
        handle.close();
        return FileHandle(-1);
    }
    
    return handle;
}

// Rotate the file for a shard if it exceeds a certain size
void FileManager::rotateIfNeeded(uint64_t shardId) {
    // Check if we have an open file for this shard
    if (shardFiles_.find(shardId) == shardFiles_.end() || shardFiles_[shardId].empty()) {
        return;
    }
    
    // Find the current file
    FileHandle& currentHandle = shardFiles_[shardId].back();
    if (!currentHandle.isValid()) {
        return;
    }
    
    // Get the current size
    struct stat st;
    if (fstat(currentHandle.getFd(), &st) < 0) {
        std::cerr << "Failed to get file stats: " << strerror(errno) << std::endl;
        return;
    }
    
    const size_t MAX_FILE_SIZE = 100 * 1024 * 1024; // 100MB, adjust as needed
    
    if (st.st_size >= MAX_FILE_SIZE) {
        // Close the current file
        currentHandle.close();
        
        // Create a new file with an incremented segment number
        uint64_t segmentNum = utils::getNextSegmentNumber(shardId, basePath_);
        std::string newFilename = utils::generateFilename(shardId, segmentNum, basePath_);
        
        int fd = open(newFilename.c_str(), O_WRONLY | O_CREAT, 0644);
        if (fd < 0) {
            std::cerr << "Failed to create new segment file: " << strerror(errno) << std::endl;
            return;
        }
        
        // Create a new handle and add it to the vector
        FileHandle newHandle(fd);
        shardFiles_[shardId].push_back(newHandle);
    }
}

// Delete old segments for a shard up to a specific offset
void FileManager::deleteOldSegments(uint64_t shardId, uint64_t upToOffset) {
    // List all segment files for this shard
    std::vector<std::string> segmentFiles;
    std::string shardPrefix = utils::getShardPrefix(shardId);
    
    for (const auto& entry : std::filesystem::directory_iterator(basePath_)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.find(shardPrefix) == 0) {
                segmentFiles.push_back(entry.path().string());
            }
        }
    }
    
    // Sort the segment files by segment number
    std::sort(segmentFiles.begin(), segmentFiles.end());
    
    // Delete segments until we reach the one containing upToOffset
    uint64_t currentOffset = 0;
    for (const auto& filename : segmentFiles) {
        struct stat st;
        if (stat(filename.c_str(), &st) < 0) {
            continue;
        }
        
        uint64_t fileSize = st.st_size;
        
        // If adding this file's size exceeds upToOffset, we keep this file and all after it
        if (currentOffset + fileSize > upToOffset) {
            break;
        }
        
        // Close any handles to this file
        for (auto it = shardFiles_[shardId].begin(); it != shardFiles_[shardId].end();) {
            // We need a way to identify which FileHandle corresponds to which file
            // This is a placeholder - might need to enhance FileHandle to store filename
            it = shardFiles_[shardId].erase(it);
        }
        
        // Delete the file
        std::filesystem::remove(filename);
        
        // Update current offset
        currentOffset += fileSize;
    }
}


FileManager::~FileManager() {}
