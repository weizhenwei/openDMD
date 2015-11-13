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

include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/src/util)

file(GLOB UNIVERSAL_FILES ./*.h ./*.cpp)
if(LINUX_PLATFORM)
    FILE(GLOB PLATFORM_SPECIFIC_FILES ./linux/*.h ./linux/*.cpp)
elseif(MAC_PLATFORM)    
    file(GLOB PLATFORM_SPECIFIC_FILES ./mac/*.h ./mac/*.cpp */mac/*.m */mac/*.mm)
endif()

set(ALL_FILES ${UNIVERSAL_FILES} ${PLATFORM_SPECIFIC_FILES})

# default is static library
add_library(capture ${ALL_FILES})
set_target_properties(capture PROPERTIES OUTPUT_NAME "capture")

