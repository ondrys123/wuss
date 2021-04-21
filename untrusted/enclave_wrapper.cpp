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

bool enclave_wrapper::delete_wallet()
{
    int ret{};
    auto status = ::delete_wallet(_eid, &ret);
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

bool enclave_wrapper::add_item(const item_t&)
{
    return false;
}

bool enclave_wrapper::delete_item(const id_t&)
{
    return false;
}

std::optional<item_t> enclave_wrapper::show_item(const id_t&)
{
    return {};
}


std::vector<item_t> enclave_wrapper::show_all_items()
{
    return {};
}

std::vector<id_t> enclave_wrapper::list_all_ids()
{
    uint32_t size{};
    auto status = get_ids_total_size(_eid, &size);
    if (status != sgx_status_t::SGX_SUCCESS)
    {
        throw std::runtime_error("error");
    }
    auto buff = std::make_unique<char[]>(size);
    int retval{};
    status = ::list_all_ids(_eid, &retval, buff.get(), size);
    if (status != sgx_status_t::SGX_SUCCESS || !retval)
    {
        throw std::runtime_error("error2");
    }
    std::vector<id_t> result;
    char* it        = buff.get();
    const char* end = buff.get() + size;
    while (it < end)
    {
        result.emplace_back(it);
        it += result.back().size() + 1;
    }
    return result;
}


/********************************************************************************/
/******************************** OCALL handlers ********************************/
/********************************************************************************/


void enclave_wrapper::on_error(const std::string& error_)
{
    std::cerr << "Error: " << error_ << std::endl;
}


} // namespace wuss
