add_library(game)

target_sources(game
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Player.cpp
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Player.h
        )

target_include_directories(game
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        )

find_package(OpenSSL 1.1.1 REQUIRED)
target_link_libraries(game PUBLIC crypto)
target_link_libraries(game PUBLIC socket)
target_link_libraries(game PRIVATE OpenSSL::Crypto)
target_link_libraries(game PRIVATE exception)
target_link_libraries(game PRIVATE utils)