add_library(game)

target_sources(game
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Player.cpp
        ${CMAKE_CURRENT_LIST_DIR}/FourInARow.cpp
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Player.h
        ${CMAKE_CURRENT_LIST_DIR}/FourInARow.h
        )

target_include_directories(game
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        )

target_link_libraries(game PUBLIC crypto)
target_link_libraries(game PUBLIC socket)
target_link_libraries(game PRIVATE exception)
target_link_libraries(game PRIVATE utils)