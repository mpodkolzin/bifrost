
add_library(io STATIC 
    buffered_writer.cpp
    file_handle.cpp
    file_manager.cpp
    shard_manager.cpp
    writer_pool.cpp
    wal_reader.cpp
    )

target_include_directories(io PUBLIC ${CMAKE_SOURCE_DIR}/include)