add_library(exception)

target_sources(exception
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/SocketException.cpp
        ${CMAKE_CURRENT_LIST_DIR}/SerializationException.cpp
        ${CMAKE_CURRENT_LIST_DIR}/CryptoException.cpp
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/SocketException.h
        ${CMAKE_CURRENT_LIST_DIR}/SerializationException.h
        ${CMAKE_CURRENT_LIST_DIR}/CryptoException.h
        )

target_include_directories(exception
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        )