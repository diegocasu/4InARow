add_library(message)

target_sources(message
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Message.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ClientHello.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ServerHello.cpp
        ${CMAKE_CURRENT_LIST_DIR}/EndHandshake.cpp
        ${CMAKE_CURRENT_LIST_DIR}/InfoMessage.cpp
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Message.h
        ${CMAKE_CURRENT_LIST_DIR}/ClientHello.h
        ${CMAKE_CURRENT_LIST_DIR}/ServerHello.h
        ${CMAKE_CURRENT_LIST_DIR}/EndHandshake.h
        ${CMAKE_CURRENT_LIST_DIR}/InfoMessage.h
        )

target_include_directories(message
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        )

target_link_libraries(message PRIVATE exception)
target_link_libraries(message PRIVATE utils)