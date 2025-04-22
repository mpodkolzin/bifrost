#include <iostream>
#include "wal/wal.h"
#include "httplib.h"
#include "io/file_manager.h"
#include "io/buffered_writer.h"
#include "wal/wal_record.h"

//int main(int, char**){
//    std::cout << "Hello, from bifrost!\n";
//    WAL wal(std::string("./wal.txt"));
//    wal.append("Hello, world!");
//    wal.flush();
//}

void handle_hello(const httplib::Request& req, httplib::Response& res) {
    res.set_content("Hello World", "text/plain");

}

std::vector<uint8_t> to_vector(const std::string& str) {
    return std::vector<uint8_t>(str.begin(), str.end());
}


int main() {
    // Create a server instance
    httplib::Server svr;

    WAL wal(std::string("./wal.txt"));

    // Define a route handler
    svr.Get("/write", [&wal](const httplib::Request& req, httplib::Response& res) {
        WALRecord record(RecordType::DATA, to_vector("Hello, world!"));
        res.set_content("Hello, World!", "text/plain");
        wal.append(record);
        wal.flush();
    });

    svr.Get("/hello", handle_hello);


    std::cout << "Server is starting at http://localhost:8080/" << std::endl;

    // Start listening on port 8080
    svr.listen("0.0.0.0", 8080);

    return 0;
}