project(wal, CXX)
#set(SOURCE_FILES wal.cpp)
add_library(wal STATIC wal.cpp wal_record.cpp)

target_include_directories(wal PUBLIC ${CMAKE_SOURCE_DIR}/include)
