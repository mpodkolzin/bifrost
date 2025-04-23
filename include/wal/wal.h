#pragma once

#include <string>
#include <fstream>
#include "io/buffered_writer.h"
#include "io/file_manager.h"
#include "io/writer_pool.h"
#include "wal/wal_record.h"
#include "io/wal_reader.h"

class WAL {
    public:
        WAL(std::string path);
        ~WAL();
        void append(const WALRecord& record);
        bool read_next(WALRecord& record);
        void flush();

    private:
        std::string path;
        std::fstream file;
        std::unique_ptr<WriterPool> writerPool_;
        std::unique_ptr<FileManager> file_manager_;
        std::unique_ptr<WALReader> wal_reader_;
        //std::unique_ptr<BufferedWriter> buffered_writer_;
};