set(target misc)
message(STATUS "${target}")

set(current_dir ${CMAKE_CURRENT_SOURCE_DIR})

set(enclave_edl_dir ${current_dir} CACHE INTERNAL "EDL directory")
set(enclave_edl_file ${current_dir}/enclave.edl CACHE INTERNAL "enclave .edl file")
set(enclave_cfg_file ${current_dir}/enclave.config.xml CACHE INTERNAL "enclave config file")
set(enclave_pkey ${current_dir}/private.pem CACHE INTERNAL "Private key for signing")

if(NOT EXISTS ${enclave_edl_file})
    message(SEND_ERROR "Missing 'enclave.edl' file")
endif()

if(NOT EXISTS ${enclave_cfg_file})
    message(SEND_ERROR "Missing 'enclave.config.xml' file")
endif()

if(NOT EXISTS ${enclave_pkey})
    message(SEND_ERROR "Missing 'private.pem' file")
endif()
