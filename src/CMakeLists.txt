set(SRC_LIST main.cpp main.h client.cpp server.cpp
        utils/DmdLog.h utils/DmdLog.cpp
        utils/DmdConfig.h utils/DmdConfig.cpp
        utils/DmdMutex.h utils/DmdMutex.cpp)
add_executable(openDMD ${SRC_LIST})
add_subdirectory(include)

