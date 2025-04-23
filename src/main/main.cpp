#include <iostream>
#include "wal/wal.h"
#include "httplib.h"
#include "io/file_manager.h"
#include "io/buffered_writer.h"
#include "wal/wal_record.h"
#include <string>
#include <ctime>
#include <iomanip>

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

std::string get_current_date_time() {
    auto now = std::chrono::system_clock::now();
    auto time_c = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&time_c);
    std::stringstream ss;
    ss << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}


int main() {
    // Create a server instance
    httplib::Server svr;

    WAL wal(std::string("./wal.txt"));

    // Define a route handler
    svr.Get("/write", [&wal](const httplib::Request& req, httplib::Response& res) {
        std::string content = "Hello, World! " + get_current_date_time();
        WALRecord record(RecordType::DATA, to_vector(content));
        res.set_content(content, "text/plain");
        wal.append(record);
        wal.flush();
    });
    svr.Get("/read", [&wal](const httplib::Request& req, httplib::Response& res) {
        std::cout << "Creating record" << std::endl;
        WALRecord record(RecordType::DATA, {});
        std::string res_content;
        if (wal.read_next(record)) {
            res_content = std::string(record.payload.begin(), record.payload.end());
        }
        else {
            res_content = "No more records";
        }
        //while(wal.read_next(record)) {
        //    std::string str(record.payload.begin(), record.payload.end());
        //    std::cout << str << std::endl;
        //    res_content += str;
        //}
        res.set_content(res_content, "text/plain");
    });


    svr.Get("/hello", handle_hello);


    std::cout << "Server is starting at http://localhost:8080/" << std::endl;

    // Start listening on port 8080
    svr.listen("0.0.0.0", 8080);

    return 0;
}