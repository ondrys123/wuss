#include "enclave_wrapper.hpp"
#include "enclave_u.h"
#include "sgx_urts.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#define WALLET_FILE "wallet.seal"


namespace wuss
{
std::optional<enclave_wrapper> enclave_wrapper::_instance = {};

enclave_wrapper::enclave_wrapper(token)
{
    const std::filesystem::path shared_lib_path{SIGNED_LIB};
    if (!std::filesystem::exists(shared_lib_path))
    {
        throw std::runtime_error{"File '" + shared_lib_path.string() + "' missing"};
    }

    sgx_launch_token_t token = {0};
    int token_updated{};

    const sgx_status_t status = sgx_create_enclave(shared_lib_path.c_str(), SGX_DEBUG_FLAG, &token, &token_updated, &_eid, NULL);
    throw_on_failure(status, "Failed to create enclave");
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
    const auto status = ::create_wallet(_eid, &ret, mp_.c_str());
    throw_on_failure(status, "Failed to create wallet");
    return ret;
}

bool enclave_wrapper::delete_wallet()
{
    int ret{};
    const auto status = ::delete_wallet(_eid, &ret);
    throw_on_failure(status, "Failed to delete enclave");
    return ret;
}

bool enclave_wrapper::check_password(const password_t& mp_)
{
    int ret{};
    const auto status = ::check_password(_eid, &ret, mp_.c_str());
    throw_on_failure(status, "Failed to check password");
    return ret;
}

bool enclave_wrapper::change_master_password(const password_t& old_mp_, const password_t& new_mp_)
{
    int ret{};
    const auto status = ::change_master_password(_eid, &ret, old_mp_.c_str(), new_mp_.c_str());
    throw_on_failure(status, "Failed to change master password");
    return ret;
}

bool enclave_wrapper::add_item(const item_t& item_)
{
    int ret{};
    const auto status = ::add_item(_eid, &ret, item_.id.c_str(), item_.username.c_str(), item_.password.c_str());
    throw_on_failure(status, "Failed to add item into wallet");
    return ret;
}

bool enclave_wrapper::delete_item(const id_t& id_)
{
    int ret{};
    const auto status = ::delete_item(_eid, &ret, id_.c_str());
    throw_on_failure(status, "Failed to delete item from wallet");
    return ret;
}

std::optional<item_t> enclave_wrapper::show_item(const id_t& id_)
{
    uint32_t max_size{};
    auto status     = ::get_max_field_size(_eid, &max_size);
    const auto buff = std::make_unique<char[]>(max_size * 2);
    throw_on_failure(status, "Failed to obtain max field size");

    int ret{};
    status = ::show_item(_eid, &ret, id_.c_str(), buff.get(), max_size * 2);
    throw_on_failure(status, "Failed to show item");
    if (!ret)
    {
        return std::nullopt;
    }

    item_t result;
    result.id       = id_;
    result.username = std::string{buff.get()};
    result.password = std::string{buff.get() + result.username.size() + 1};
    return result;
}


std::vector<item_t> enclave_wrapper::show_all_items()
{
    uint32_t size{};
    auto status = ::get_wallet_total_size(_eid, &size);
    throw_on_failure(status, "Failed to obtain size of all items in wallet");
    const auto buff = std::make_unique<char[]>(size);
    int ret{};
    status = ::show_all_items(_eid, &ret, buff.get(), size);
    if (!ret)
    {
        return {};
    }
    throw_on_failure(status, "Failed to obtain wallet items");
    std::vector<item_t> result;
    const char* it        = buff.get();
    const char* const end = buff.get() + size;
    while (it < end)
    {
        item_t item;
        item.id = std::string{it};
        it += item.id.size() + 1;
        item.username = std::string{it};
        it += item.username.size() + 1;
        item.password = std::string{it};
        it += item.password.size() + 1;
        result.emplace_back(std::move(item));
    }
    return result;
}

std::vector<id_t> enclave_wrapper::list_all_ids()
{
    uint32_t size{};
    auto status = ::get_ids_total_size(_eid, &size);
    throw_on_failure(status, "Failed to obtain size of all IDs");

    const auto buff = std::make_unique<char[]>(size);
    int ret{};
    status = ::list_all_ids(_eid, &ret, buff.get(), size);
    throw_on_failure(status, "Failed to obtain IDs");

    std::vector<id_t> result;
    const char* it        = buff.get();
    const char* const end = buff.get() + size;
    while (it < end)
    {
        result.emplace_back(it);
        it += result.back().size() + 1;
    }
    return result;
}

/********************************************************************************/
/******************************* Private methods ********************************/
/********************************************************************************/

void enclave_wrapper::throw_on_failure(sgx_status_t status_, const std::string& error_msg_)
{
    if (status_ != sgx_status_t::SGX_SUCCESS)
    {
        std::stringstream ss;
        ss << error_msg_ << " (Error code 0x" << std::hex << std::setw(4) << std::setfill('0') << +status_ << ")";
        throw std::runtime_error{ss.str()};
    }
}


/********************************************************************************/
/******************************** OCALL handlers ********************************/
/********************************************************************************/

void enclave_wrapper::on_error(const std::string& error_)
{
    std::cerr << "Error: " << error_ << std::endl;
}

int enclave_wrapper::store_wallet(const uint8_t* sealed_data, const size_t sealed_size)
{
    std::ofstream file(WALLET_FILE, std::ios::out | std::ios::binary);
    if (file.fail())
    {
        return 1;
    }
    file.write((const char*)sealed_data, sealed_size);
    return 0;
}


size_t enclave_wrapper::get_file_size()
{
    if (std::filesystem::exists(WALLET_FILE))
    {
        return std::filesystem::file_size(WALLET_FILE);
    }
    return 0;
}

int enclave_wrapper::load_wallet(uint8_t* sealed_data, const size_t sealed_size)
{
    std::ifstream file(WALLET_FILE, std::ios::in | std::ios::binary);
    if (file.fail())
    {
        return 1;
    }
    file.read((char*)sealed_data, sealed_size);
    return 0;
}
} // namespace wuss