message(STATUS "Entering directory ${CMAKE_CURRENT_SOURCE_DIR}")

# detect platform;
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        set(LINUX_PLATFORM TRUE)
    endif()
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    IF(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        set(MAC_PLATFORM TRUE)
    endif()
endif()
if(NOT LINUX_PLATFORM AND NOT MAC_PLATFORM)
    message(FATAL_ERROR "Only Linux-x86_64 and Darwin-x86_64 platform supported")
endif()

file(GLOB UNIVERSAL_FILES ./*.h ./*.cpp)
file(GLOB INCLUDE_FILES ${PROJECT_SOURCE_DIR}/src/include/*.h)
if(LINUX_PLATFORM)
    include_directories(${PROJECT_SOURCE_DIR}/src/capture/linux)
    FILE(GLOB PLATFORM_SPECIFIC_FILES ./linux/*.h ./linux/*.cpp)
elseif(MAC_PLATFORM)    
    include_directories(${PROJECT_SOURCE_DIR}/src/capture/mac)
    file(GLOB PLATFORM_SPECIFIC_FILES ./mac/*.h ./mac/*.cpp ./mac/*.m ./mac/*.mm)
endif()
set(ALL_FILES ${UNIVERSAL_FILES} ${INCLUDE_FILES} ${PLATFORM_SPECIFIC_FILES})

# default is static library
add_library(capture SHARED ${ALL_FILES})
set_target_properties(capture PROPERTIES OUTPUT_NAME "capture")

# dependent libraries
if(LINUX_PLATFORM)
    set(PLATFORM_LIB "")
elseif(MAC_PLATFORM)
    find_library(COCOA_LIB Cocoa)
    find_library(FOUNDATION_LIB Foundation)
    find_library(AVFOUNDATION_LIB AVFoundation)
    find_library(COREFOUNDATION_LIB CoreFoundation)
    find_library(COREMEDIA_LIB CoreMedia)
    find_library(COREVIDEO_LIB CoreVideo)
    set(PLATFORM_LIB ${COCOA_LIB} ${FOUNDATION_LIB} ${AVFOUNDATION_LIB}
            ${COREFOUNDATION_LIB} ${COREMEDIA_LIB} ${COREVIDEO_LIB})
endif()
target_link_libraries(capture util glog ${PLATFORM_LIB})

message(STATUS "Leaving directory ${CMAKE_CURRENT_SOURCE_DIR}")

