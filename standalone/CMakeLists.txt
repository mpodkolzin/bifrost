cmake_minimum_required(VERSION 3.14...3.22)

project(BifrostStandalone LANGUAGES CXX)

# --- Import tools ----

include(../cmake/tools.cmake)

# ---- Dependencies ----

include(../cmake/CPM.cmake)

CPMAddPackage(
  GITHUB_REPOSITORY jarro2783/cxxopts
  VERSION 3.0.0
  OPTIONS "CXXOPTS_BUILD_EXAMPLES NO" "CXXOPTS_BUILD_TESTS NO" "CXXOPTS_ENABLE_INSTALL YES"
)

CPMAddPackage(
  NAME httplib
  GIT_TAG v0.11.2
  GITHUB_REPOSITORY yhirose/cpp-httplib
  #OPTIONS "FMT_INSTALL YES" # create an installable target
)

CPMAddPackage(
  NAME spdlog
  GIT_TAG v1.11.0
  GITHUB_REPOSITORY gabime/spdlog
  #OPTIONS "FMT_INSTALL YES" # create an installable target
)

#if (httplib_ADDED) 
#  target_include_directories(httplib SYSTEM INTERFACE ${httplib_SOURCE_DIR})
#endif()

find_path(ROCKSDB_INCLUDE_DIR rocksdb/db.h
    PATHS "")

include_directories(${ROCKSDB_INCLUDE_DIR})

find_library(ROCKSDB_LIB rocksdb)
message(STATUS "finding ${ROCKSDB_LIB}")


find_package(Boost REQUIRED COMPONENTS system)



#CPMAddPackage(
#  NAME rocksdb
#  GIT_TAG v7.5.3
#  GITHUB_REPOSITORY facebook/rocksdb
#)

CPMAddPackage(NAME Bifrost SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

# ---- Create standalone executable ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)


add_executable(${PROJECT_NAME} ${sources})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "bifrost")

if(Boost_FOUND)
    message("Boost found")
    include_directories(${Boost_INCLUDE_DIRS}) 
    target_link_libraries(${PROJECT_NAME} Bifrost::Bifrost ${Boost_LIBRARIES})
endif()

#target_link_libraries(${PROJECT_NAME} INTERFACE httplib)
target_link_libraries(${PROJECT_NAME} Bifrost::Bifrost ${ROCKSDB_LIB} httplib spdlog)
#target_link_libraries(${PROJECT_NAME} Greeter::Greeter cxxopts)
