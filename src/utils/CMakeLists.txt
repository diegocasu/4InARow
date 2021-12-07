add_library(utils)

target_sources(utils
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Utils.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Constants.cpp
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Utils.h
        ${CMAKE_CURRENT_LIST_DIR}/Constants.h
        )

target_include_directories(utils
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        )

find_package(OpenSSL 1.1.1 REQUIRED)
target_link_libraries(utils PRIVATE OpenSSL::Crypto)