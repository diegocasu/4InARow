set(HEADER_FILES
        ${CMAKE_CURRENT_LIST_DIR}/handler/Handler.h
        ${CMAKE_CURRENT_LIST_DIR}/handler/HandshakeHandler.h
        ${CMAKE_CURRENT_LIST_DIR}/handler/PreGameHandler.h
        )

set(SOURCE_FILES
        ${CMAKE_CURRENT_LIST_DIR}/handler/Handler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/handler/HandshakeHandler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/handler/PreGameHandler.cpp
        )

add_executable(client main.cpp ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(client
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/handler
        )

target_link_libraries(client PRIVATE crypto)
target_link_libraries(client PRIVATE exception)
target_link_libraries(client PRIVATE game)
target_link_libraries(client PRIVATE message)
target_link_libraries(client PRIVATE socket)
target_link_libraries(client PRIVATE utils)

add_custom_command(
        TARGET client
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/keys"
        "$<TARGET_FILE_DIR:client>/keys"
)

add_custom_command(
        TARGET client
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/certificates"
        "$<TARGET_FILE_DIR:client>/certificates"
)