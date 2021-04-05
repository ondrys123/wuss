set(target untrusted)
message(STATUS "${target}")

set(headers
    ${CMAKE_CURRENT_SOURCE_DIR}/enclave_wrapper.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/io_handler.hpp
)

set(sources
    ${CMAKE_CURRENT_SOURCE_DIR}/enclave_wrapper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/io_handler.cpp
)

set(${target}_files ${sources} ${header} PARENT_SCOPE)
