add_library(socket)

target_sources(socket
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/TcpSocket.cpp
        ${CMAKE_CURRENT_LIST_DIR}/InputMultiplexing.cpp
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/TcpSocket.h
        ${CMAKE_CURRENT_LIST_DIR}/InputMultiplexing.h
        )

target_include_directories(socket
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        )

target_link_libraries(socket PRIVATE exception)
target_link_libraries(socket PRIVATE utils)