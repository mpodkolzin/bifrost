#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>

class FileHandle {
public:
    explicit FileHandle(int fd);
    ~FileHandle();

    ssize_t write(const void* data, size_t size);
    ssize_t read(void* buffer, size_t size);
    off_t seek(off_t offset, int whence);
    void flush();
    void close();

    bool isValid() const;

    int getFd();


private:
    int fd_;
};
