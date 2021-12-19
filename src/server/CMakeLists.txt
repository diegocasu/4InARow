set(HEADER_FILES
        ${CMAKE_CURRENT_LIST_DIR}/handler/Handler.h
        ${CMAKE_CURRENT_LIST_DIR}/handler/NewClientHandler.h
        ${CMAKE_CURRENT_LIST_DIR}/handler/ConnectedClientHandler.h
        ${CMAKE_CURRENT_LIST_DIR}/handler/HandshakeClientHandler.h
        ${CMAKE_CURRENT_LIST_DIR}/handler/AvailableClientHandler.h
        ${CMAKE_CURRENT_LIST_DIR}/handler/MatchmakingClientHandler.h
        ${CMAKE_CURRENT_LIST_DIR}/handler/PlayingClientHandler.h
        )

set(SOURCE_FILES
        ${CMAKE_CURRENT_LIST_DIR}/handler/Handler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/handler/NewClientHandler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/handler/ConnectedClientHandler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/handler/HandshakeClientHandler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/handler/AvailableClientHandler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/handler/MatchmakingClientHandler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/handler/PlayingClientHandler.cpp
        )

add_executable(server main.cpp ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(server
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/handler
        )

target_link_libraries(server PRIVATE crypto)
target_link_libraries(server PRIVATE exception)
target_link_libraries(server PRIVATE game)
target_link_libraries(server PRIVATE message)
target_link_libraries(server PRIVATE socket)
target_link_libraries(server PRIVATE utils)

add_custom_command(
        TARGET server
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/certificate"
        "$<TARGET_FILE_DIR:server>/certificate"
)

add_custom_command(
        TARGET server
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/players"
        "$<TARGET_FILE_DIR:server>/players"
)