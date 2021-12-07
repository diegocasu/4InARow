add_library(crypto)

target_sources(crypto
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/DiffieHellman.cpp
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/DiffieHellman.h
        )

target_include_directories(crypto
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        )

find_package(OpenSSL 1.1.1 REQUIRED)
target_link_libraries(crypto PRIVATE OpenSSL::Crypto)
target_link_libraries(crypto PRIVATE exception)
target_link_libraries(crypto PRIVATE utils)