#include "io/file_handle.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include <iostream>

FileHandle::FileHandle(int fd) : fd_(fd) {}
    
FileHandle::~FileHandle() {
    if (isValid()) {
        close();
    }
}
    
ssize_t FileHandle::write(const void* data, size_t size) {
    if (!isValid()) {
        return -1;
    }
    return ::write(fd_, data, size);
}

ssize_t FileHandle::read(void* buffer, size_t size) {
    if (!isValid()) {
        return -1;
    }
    return ::read(fd_, buffer, size);
}

off_t FileHandle::seek(off_t offset, int whence) {
    if (!isValid()) {
        return -1;
    }
    return ::lseek(fd_, offset, whence);
}

void FileHandle::flush() {
    if (isValid()) {
        // On Unix-like systems, fsync() flushes the file to disk
        ::fsync(fd_);
    }
}

void FileHandle::close() {
    if (isValid()) {
        ::close(fd_);
        fd_ = -1;
    }
}

bool FileHandle::isValid() const {
    return fd_ >= 0;
}

int FileHandle::getFd() {
    return fd_;
}
