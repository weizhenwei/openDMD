set(SRC_LIST main.cpp main.h client.cpp server.cpp
        utils/DmdLog.h utils/DmdLog.cpp
        utils/DmdConfig.h utils/DmdConfig.cpp
        utils/DmdMutex.h utils/DmdMutex.cpp)

MESSAGE(STATUS "PROJECT_SOURCE_DIR is ${PROJECT_SOURCE_DIR}")

set(GLOG_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/glog/include)
set(GLOG_LIB_DIR ${PROJECT_SOURCE_DIR}/vendor/glog/lib)

LINK_DIRECTORIES(${GLOG_LIB_DIR})
INCLUDE_DIRECTORIES(${GLOG_INCLUDE_DIR})
MESSAGE(STATUS "GLOG_INCLUDE_DIR is ${GLOG_INCLUDE_DIR}")
MESSAGE(STATUS "GLOG_LIB_DIR is ${GLOG_LIB_DIR}")

add_executable(openDMD ${SRC_LIST})
TARGET_LINK_LIBRARIES(openDMD glog)
add_subdirectory(include)

