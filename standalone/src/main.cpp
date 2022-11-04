//
// Copyright (c) 2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: HTTP server, small
//
//------------------------------------------------------------------------------

#include "httplib.h"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "bifrost/bifrost.h"
//shit practice
#include  "../../src/wal/writer.cpp"

int
main(int argc, char* argv[])
{
    auto logger = spdlog::stdout_color_mt("console");

    logger->info("hi");
    // HTTP
    httplib::Server svr;

    bifrost::Bifrost greeter {"Max"};
    
    wal::WalWriter writer("test1.txt");
    writer.open_file();
    writer.write(0, "hi there");
    writer.flush();
    writer.close_file();
    
    bifrost::LanguageCode code {bifrost::LanguageCode::DE};

    svr.Get("/hi", [greeter, code](const httplib::Request &, httplib::Response &res) {
        res.set_content(greeter.greet(code), "text/plain");
    });

    svr.listen("0.0.0.0", 8080);
}
