#include "enclave_wrapper.hpp"
#include "enclave_u.h"
#include "sgx_error.h"
#include "sgx_urts.h"
#include <filesystem>
#include <iostream>
#include <sstream>

std::optional<enclave_wrapper> enclave_wrapper::_instance = {};

enclave_wrapper::enclave_wrapper(token)
{
    const std::filesystem::path shared_lib_path{SIGNED_LIB};
    if (!std::filesystem::exists(shared_lib_path))
    {
        throw std::runtime_error{"Error: File '" + shared_lib_path.string() + "' missing"};
    }

    sgx_launch_token_t token = {0};
    int token_updated{};

    const sgx_status_t status = sgx_create_enclave(shared_lib_path.c_str(), SGX_DEBUG_FLAG, &token, &token_updated, &_enclave_id, NULL);
    if (status != sgx_status_t::SGX_SUCCESS)
    {
        std::stringstream ss;
        ss << "Error: Failed to create enclave - status 0x" << std::hex << status;
        throw std::runtime_error{ss.str()};
    }
}

enclave_wrapper& enclave_wrapper::get_instance()
{
    if (!_instance)
        _instance.emplace(enclave_wrapper::token{});
    return *_instance;
}

int enclave_wrapper::call_test(int i_)
{
    std::cout << "calling 'test' with arg " << i_ << std::endl;
    int ret_val = -1;
    auto status = test(_enclave_id, &ret_val, i_);
    std::cout << (status == sgx_status_t::SGX_SUCCESS ? "Success" : "Fail") << std::endl;
    return ret_val;
}

/********************************************************************************/
/******************************** OCALL handlers ********************************/
/********************************************************************************/

void enclave_wrapper::print_int_impl(int i_)
{
    std::cout << "#### Printing argument from enclave: i=" << i_ << " ####" << std::endl;
}
