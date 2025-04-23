#include <iostream>
#include <fstream>
#include <string>
#include "wal/wal.h"
#include "io/buffered_writer.h"
#include "io/file_manager.h"
#include "io/writer_pool.h"

WAL::WAL(std::string path) {
    file.open(path, std::ios::out | std::ios::app | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    writerPool_ = std::make_unique<WriterPool>();
    file_manager_ = std::make_unique<FileManager>(std::string("./data/wal"));
    wal_reader_ = std::make_unique<WALReader>(file_manager_->getActiveWriteHandle(), 4096);
}

WAL::~WAL() {
    file.close();
}

void WAL::append(const WALRecord& record) {
    auto wal_shard = file_manager_->getActiveWriteHandle();
    auto writer = writerPool_->getWriterForShard(0, wal_shard);
    writer->write(record);
}

bool WAL::read_next(WALRecord& record) {
    std::cout << "Reading next record" << std::endl;
    return wal_reader_->next(record);
}

void WAL::flush() {
    file.flush();
}
