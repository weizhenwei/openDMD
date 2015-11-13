message(STATUS "Entering directory ${CMAKE_CURRENT_SOURCE_DIR}")

# detect platform;
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        set(LINUX_PLATFORM TRUE)
    endif()
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
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
include_directories(${PROJECT_SOURCE_DIR}/src/util)

if(LINUX_PLATFORM)
    include_directories(${PROJECT_SOURCE_DIR}/vendor/glog/linux-x86_64/include)
    link_directories(${PROJECT_SOURCE_DIR}/vendor/glog/linux-x86_64/lib)
elseif(MAC_PLATFORM)    
    include_directories(${PROJECT_SOURCE_DIR}/vendor/glog/mac-x86_64/include)
    link_directories(${PROJECT_SOURCE_DIR}/vendor/glog/mac-x86_64/lib)
endif()

# main source files;
file(GLOB UNIVERSAL_FILES main/main.h main/main.cpp)
if(LINUX_PLATFORM)
    file(GLOB PLATFORM_SPECIFIC_FILES main/client_linux.cpp)
    set(PLATFORM_LIB "")
elseif(MAC_PLATFORM)
    file(GLOB PLATFORM_SPECIFIC_FILES main/client_mac.mm)
    find_library(COCOA_LIB Cocoa)
    find_library(FOUNDATION_LIB Foundation)
    find_library(AVFOUNDATION_LIB AVFoundation)
    set(PLATFORM_LIB ${COCOA_LIB} ${FOUNDATION_LIB} ${AVFOUNDATION_LIB})
endif()

set(ALL_FILES ${UNIVERSAL_FILES} ${PLATFORM_SPECIFIC_FILES})
add_executable(openDMD ${ALL_FILES})

# libraries
add_subdirectory(capture)
add_subdirectory(util)
link_directories(${PROJECT_SOURCE_DIR}/src/capture)
link_directories(${PROJECT_SOURCE_DIR}/src/util)
target_link_libraries(openDMD glog capture util ${PLATFORM_LIB}) 

