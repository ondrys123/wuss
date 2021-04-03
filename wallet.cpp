#include "enclave_t.h"
//#include <iostream>
#include <numeric>
#include <vector>

int test(int test_input_)
{
    if (const auto status = print_int(test_input_); status != sgx_status_t::SGX_SUCCESS)
    {
        return -status;
    }

    std::vector v{test_input_, test_input_, test_input_};
    return std::accumulate(v.begin(), v.end(), 0);
}
