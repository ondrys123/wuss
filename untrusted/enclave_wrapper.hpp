#pragma once

#include <sgx_eid.h>

class enclave_wrapper
{
public:
    enclave_wrapper(sgx_enclave_id_t enclave_id_);
    int call_test(int i_);

private:
    sgx_enclave_id_t _enclave_id{};
};
