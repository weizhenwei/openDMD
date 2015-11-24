project(openDMD)

cmake_minimum_required(VERSION 2.8)

message(STATUS "PROJECT_SOURCE_DIR is ${PROJECT_SOURCE_DIR}")
message(STATUS "PROJECT_BINARY_DIR is ${PROJECT_SOURCE_DIR}")
message(STATUS "CMAKE_CXX_COMPILER_ID is ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR is ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_SYSTEM_NAME is ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR is ${CMAKE_SYSTEM_PROCESSOR}")

option(test "Enable unit test" OFF)

# enable gtest unittest;
# enable_testing()

# set CPP flags;
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -Wall")

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DLINUX)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_definitions(-DMACOSX)
    # enable gtest unittest, temprory;
    enable_testing()
endif()

add_subdirectory(src)
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_subdirectory(unittest)
endif()

