#pragma once

#include <string>
#include <fstream>
#include <io/buffered_writer.h>
#include <io/file_manager.h>

class WAL {
    public:
        WAL(std::string path);
        ~WAL();
        void append(const std::string& data);
        void flush();

    private:
        std::string path;
        std::fstream file;
        std::unique_ptr<BufferedWriter> buffered_writer_;
        std::unique_ptr<FileManager> file_manager_;
};