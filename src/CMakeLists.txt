cmake_minimum_required(VERSION 3.10.0)
project(bifrost VERSION 0.1.0 LANGUAGES C CXX)

#add_library(MyLib STATIC
#    foo.cpp
#)

#target_include_directories(bifrost PUBLIC
#    ${PROJECT_SOURCE_DIR}/include
#)

add_subdirectory(main)
add_subdirectory(wal)
add_subdirectory(io)



