add_library(exception)

target_sources(exception
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/SocketException.cpp
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/SocketException.h
        )

target_include_directories(exception
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        )