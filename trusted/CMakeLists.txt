set(target trusted)
message(STATUS "${target}")

set(signed_lib ${target}.signed.so CACHE INTERNAL "Name of signed trusted library")
set(signed_target ${target}-sign CACHE INTERNAL "asd")

set(sources
    ${CMAKE_CURRENT_SOURCE_DIR}/wallet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/storage_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ecalls.cpp
)

set(headers
    ${CMAKE_CURRENT_SOURCE_DIR}/wallet.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/storage_handler.hpp
)

add_enclave_library(${target}
    SRCS ${sources} ${headers}
    EDL ${enclave_edl_file}
    EDL_SEARCH_PATHS ${enclave_edl_dir}
)

enclave_sign(${target}
    OUTPUT ${trusted_lib}
    KEY ${enclave_pkey}
    CONFIG ${enclave_cfg_file}
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
