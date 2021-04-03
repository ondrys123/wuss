cmake_minimum_required(VERSION 3.16.3 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(wuss 
    VERSION 0.0.1 
    DESCRIPTION  "PV204 Semestral project 2021"
    HOMEPAGE_URL "https://github.com/ondrys123/wuss"
    LANGUAGES C CXX
)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(SGX_HW OFF CACHE BOOL "Enable HW mode" FORCE)
set(SGX_MODE "Debug" CACHE STRING "SGX mode" FORCE)
if(DEFINED SGX_DIR)
    set(ENV{SGX_SDK} ${SGX_DIR})
endif()
find_package(SGX REQUIRED)

set(test_enclave_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/wallet.cpp
    )

add_enclave_library(test_enclave_lib
    SRCS ${test_enclave_sources}
    EDL enclave.edl
    EDL_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SGX_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/Enclave.config.xml)
enclave_sign(test_enclave_lib KEY private.pem CONFIG ${SGX_CONFIG})

set(untrusted_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/enclave_wrapper.cpp
    )

add_untrusted_executable(main
    SRCS ${untrusted_sources}
    EDL enclave.edl
    EDL_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}
)

add_dependencies(main test_enclave_lib-sign)
target_compile_definitions(main PUBLIC ENCLAVE_FILE="test_enclave_lib.signed.so")

# Unused variables
if(QT_QMAKE_EXECUTABLE)
endif()
