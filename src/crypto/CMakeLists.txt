add_library(crypto)

target_sources(crypto
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/DiffieHellman.cpp
        ${CMAKE_CURRENT_LIST_DIR}/SHA256.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DigitalSignature.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Certificate.cpp
        ${CMAKE_CURRENT_LIST_DIR}/CertificateStore.cpp
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/DiffieHellman.h
        ${CMAKE_CURRENT_LIST_DIR}/SHA256.h
        ${CMAKE_CURRENT_LIST_DIR}/DigitalSignature.h
        ${CMAKE_CURRENT_LIST_DIR}/Certificate.h
        ${CMAKE_CURRENT_LIST_DIR}/CertificateStore.h
        )

target_include_directories(crypto
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        )

find_package(OpenSSL 1.1.1 REQUIRED)
target_link_libraries(crypto PRIVATE OpenSSL::Crypto)
target_link_libraries(crypto PRIVATE exception)
target_link_libraries(crypto PRIVATE utils)