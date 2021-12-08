add_library(message)

target_sources(message
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Message.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ClientHello.cpp
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Message.h
        ${CMAKE_CURRENT_LIST_DIR}/ClientHello.h
        )

target_include_directories(message
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        )

target_link_libraries(message PRIVATE exception)
target_link_libraries(message PRIVATE utils)