#include "enclave_wrapper.hpp"
#include "enclave_u.h"
#include "sgx_error.h"
#include "sgx_urts.h"
#include <filesystem>
#include <iostream>
#include <sstream>

namespace wuss
{
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

    const sgx_status_t status = sgx_create_enclave(shared_lib_path.c_str(), SGX_DEBUG_FLAG, &token, &token_updated, &_eid, NULL);
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

enclave_wrapper::~enclave_wrapper()
{
    sgx_destroy_enclave(_eid);
}


/********************************************************************************/
/******************************** Public methods ********************************/
/********************************************************************************/

bool enclave_wrapper::create_wallet(const password_t& mp_)
{
    int ret{};
    auto status = ::create_wallet(_eid, &ret, mp_.c_str());
    if (status != sgx_status_t::SGX_SUCCESS)
    {
        throw std::runtime_error("ASDASDAS");
    }
    return ret;
}

bool enclave_wrapper::delete_wallet(const password_t& mp_)
{
    int ret{};
    auto status = ::delete_wallet(_eid, &ret, mp_.c_str());
    if (status != sgx_status_t::SGX_SUCCESS)
    {
        throw std::runtime_error("ASDASDAS");
    }
    return ret;
}

bool enclave_wrapper::check_password(const password_t& mp_)
{
    int ret{};
    auto status = ::check_password(_eid, &ret, mp_.c_str());
    if (status != sgx_status_t::SGX_SUCCESS)
    {
        throw std::runtime_error("ASDASDAS");
    }
    return ret;
}

bool enclave_wrapper::change_master_password(const password_t& old_mp_, const password_t& new_mp_)
{
    int ret{};
    auto status = ::change_master_password(_eid, &ret, old_mp_.c_str(), new_mp_.c_str());
    if (status != sgx_status_t::SGX_SUCCESS)
    {
        throw std::runtime_error("ASDASDAS");
    }
    return ret;
}

bool enclave_wrapper::add_item(const item&)
{
    return false;
}

bool enclave_wrapper::delete_item(const id_t&)
{
    return false;
}

std::optional<item> enclave_wrapper::show_item(const id_t&)
{
    return {};
}


std::vector<item> enclave_wrapper::show_all_items()
{
    return {};
}

std::vector<id_t> enclave_wrapper::list_all_ids()
{
    return {};
}


/********************************************************************************/
/******************************** OCALL handlers ********************************/
/********************************************************************************/


void enclave_wrapper::print_error_message(const std::string& error_)
{
    std::cerr << "Error: " << error_ << std::endl;
}


} // namespace wuss
