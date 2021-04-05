#include "sgx_urts.h"
#include "untrusted/enclave_wrapper.hpp"

#include <iomanip>
#include <iostream>

int main()
{
    sgx_enclave_id_t eid     = 0;
    sgx_launch_token_t token = {0};
    int updated{};
    std::cout << "Trying to create enclave from '" << SIGNED_LIB << "'" << std::endl;
    const auto enclave_status = sgx_create_enclave(SIGNED_LIB, SGX_DEBUG_FLAG, &token, &updated, &eid, NULL);

    if (enclave_status != SGX_SUCCESS)
    {
        std::cout << "Failed to create enclave - 0x" << std::hex << enclave_status << std::endl;
        return -1;
    }
    enclave_wrapper w{eid};
    const int res = w.call_test(42);
    std::cout << "call_test returned " << res << std::endl;
    return 0;
}
