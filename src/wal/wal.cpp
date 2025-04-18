#include <iostream>
#include <fstream>
#include <string>
#include "wal/wal.h"
#include "io/buffered_writer.h"
#include "io/file_manager.h"
#include "io/writer_pool.h"

WAL::WAL(std::string path) {
    //path = "./wal.txt";
    file.open(path, std::ios::out | std::ios::app | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    writerPool_ = std::make_unique<WriterPool>();
    file_manager_ = std::make_unique<FileManager>(std::string("./"));

}

WAL::~WAL() {
    file.close();
}

void WAL::append(const std::string& data) {
    file << data << std::endl;
    std::cout << "Appended data: " << data << std::endl;
}

void WAL::flush() {
    file.flush();
}
