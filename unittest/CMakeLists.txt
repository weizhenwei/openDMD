message(STATUS "Entering directory ${CMAKE_CURRENT_SOURCE_DIR}")

# detect platform;
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        set(LINUX_PLATFORM TRUE)
    endif()
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    # for eliminating the macosx_rpath warning;
    set(CMAKE_MACOSX_RPATH 1)

    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        set(MAC_PLATFORM TRUE)
    endif()
endif()

if(NOT LINUX_PLATFORM AND NOT MAC_PLATFORM)
    message(FATAL_ERROR "Only Linux-x86_64 and Darwin-x86_64 platform supported")
endif()

# include directory;
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/src/capture)
link_directories(${PROJECT_SOURCE_DIR}/src/capture)

if(LINUX_PLATFORM)
    include_directories(${PROJECT_SOURCE_DIR}/vendor/glog/linux-x86_64/include)
    link_directories(${PROJECT_SOURCE_DIR}/vendor/glog/linux-x86_64/lib)
    include_directories(${PROJECT_SOURCE_DIR}/vendor/gtest/linux-x86_64/include)
    link_directories(${PROJECT_SOURCE_DIR}/vendor/gtest/linux-x86_64/lib)
elseif(MAC_PLATFORM)    
    include_directories(${PROJECT_SOURCE_DIR}/vendor/glog/mac-x86_64/include)
    link_directories(${PROJECT_SOURCE_DIR}/vendor/glog/mac-x86_64/lib)
    include_directories(${PROJECT_SOURCE_DIR}/vendor/gtest/mac-x86_64/include)
    link_directories(${PROJECT_SOURCE_DIR}/vendor/gtest/mac-x86_64/lib)
endif()

file(GLOB CAPTURE_TESTFILES capture/*.cpp)
file(GLOB FOO_TESTFILES foo/*.h foo/*.cpp)
file(GLOB MAIN_TESTFILES main_test.cpp)


add_executable(runUnitTests ${MAIN_TESTFILES}
    ${CAPTURE_TESTFILES} ${FOO_TESTFILES})
target_link_libraries(runUnitTests gtest gtest_main pthread capture)

add_test(NAME runUnitTests COMMAND runUnitTests)


message(STATUS "Leaving directory ${CMAKE_CURRENT_SOURCE_DIR}")

