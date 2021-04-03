#include "enclave_wrapper.hpp"
#include "enclave_u.h"
#include <iostream>

void print_int(int i_)
{
    std::cout << "#### Printing argument from enclave: i=" << i_ << " ####" << std::endl;
}

enclave_wrapper::enclave_wrapper(sgx_enclave_id_t enclave_id_)
    : _enclave_id{enclave_id_}
{
    std::cout << "Initialized wrapper with enclave_id=" << _enclave_id << std::endl;
}

int enclave_wrapper::call_test(int i_)
{
    std::cout << "calling 'test' with arg " << i_ << std::endl;
    int ret_val = -1;
    auto status = test(_enclave_id, &ret_val, i_);
    std::cout << (status == sgx_status_t::SGX_SUCCESS ? "Success" : "Fail") << std::endl;
    return ret_val;
}
