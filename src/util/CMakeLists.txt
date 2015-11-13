message(STATUS "Entering directory ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB UNIVERSAL_FILES ./*.h ./*.cpp)
set(ALL_FILES ${UNIVERSAL_FILES})

# default is static library
add_library(util SHARED ${ALL_FILES})
set_target_properties(util PROPERTIES OUTPUT_NAME "util")
target_link_libraries(util glog)

