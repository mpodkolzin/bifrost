#include <iostream>
#include <string>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;
typedef std::shared_ptr<spdlog::logger> sptr_logger;

namespace wal {

class WalWriter {
private:
    string _file_name;
    sptr_logger _logger;
    size_t _fd;
    size_t _size;
    char* _memory_buffer;
    size_t _memory_buffer_size;

public:
    WalWriter(string file_name) {
        _file_name = file_name;
        _logger = spdlog::get("console");
        _size = 4096;
        _memory_buffer_size = 4096;
    }

    int open_file() {
        _fd = open(_file_name.c_str(), O_CREAT | O_RDWR,
                      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
                    
        _logger->info("Opened file: " + _fd);

        map_file(4096);

        return _fd;
    }

    //TODO add error handling
    void map_file(size_t length)
    {
        int fd = open(_file_name.c_str(), O_CREAT | O_RDWR,
                 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
        //if (fd == -1)
        //    handle_error("open");

        // obtain file size
        //struct stat sb;
        //if (fstat(fd, &sb) == -1)
        //    handle_error("fstat");

        //length = sb.st_size;

        _memory_buffer = reinterpret_cast< char * >(
            mmap(nullptr, _memory_buffer_size, PROT_WRITE, MAP_SHARED, _fd, 0));
        if (_memory_buffer == MAP_FAILED){
            _logger->error("mmap");
            abort();
        }

        if (ftruncate(_fd, _memory_buffer_size) != 0) {
            std::cerr << "Error reserving file disk space!" << std::endl;
            abort();
        }
    }
    void flush() {
        msync(_memory_buffer, 4096, MS_SYNC);
    }

    void write(const size_t position, const string value) {
        const size_t vsize = sizeof(value);
        //myassert(position < _memory_buffer_size,
        //        "Offset too large (" << position << ", " << _memory_buffer_size
        //                            << ")!");
        _logger->info("Trying to write to file = " + std::to_string(vsize));
        try {
            memcpy(_memory_buffer + position, &value, vsize);
        } catch (int ex) {
            _logger->error(std::to_string(ex));
        }
        _logger->info("Successfully written to file");
    }
    void close_file() {
    // unmap the actively memory-mapped region of the file
        if (munmap(_memory_buffer, _size) != 0) {
            std::cerr << "Error unmapping file memory!" << std::endl;
            abort();
        }
        _memory_buffer = nullptr;
        // shrink the file to its actual size
        if (ftruncate(_fd, _size) != 0) {
            std::cerr << "Error shrinking file!" << std::endl;
            abort();
        }
        // close the file
        if (close(_fd) != 0) {
            std::cerr << "Error closing file!" << std::endl;
            abort();
        }
        _fd= 0;
    }


    ~WalWriter() {

    }

};

};