#include "wallet.hpp"
#include "enclave_t.h"
#include "storage_handler.hpp"
#include <numeric>

namespace wuss
{
std::unique_ptr<wallet> wallet::_instance = nullptr;

wallet::wallet(wallet::token)
    : _state{state::not_loaded}
{
    size_t file_size{};
    const auto status = get_file_size(&file_size);
    if (status != SGX_SUCCESS)
    {
        on_error("[wallet] Failed to obtain file size");
        return;
    }
    if (file_size > 0 && load_stored_wallet())
    {
        _state = state::loaded;
    }
}

wallet& wallet::get_instance()
{
    if (!_instance)
        _instance = std::unique_ptr<wallet>(new wallet(wallet::token{}));
    return *_instance;
}

/********************************************************************************/
/******************************** Public methods ********************************/
/********************************************************************************/

bool wallet::create_wallet(const password_t& mp_)
{
    size_t size{};
    const auto status = get_file_size(&size);
    if (status != SGX_SUCCESS)
    {
        on_error("[create_wallet] Failed to access wallet");
        return false;
    }

    if (size != 0)
    {
        on_error("[create_wallet] Wallet is already created");
        return false;
    }

    if (_state != state::not_loaded)
    {
        on_error("[create_wallet] Wallet is already loaded");
        return false;
    }

    _master_password = mp_;
    _items.clear();
    _state = state::open;
    update_stored_wallet();
    return true;
}

bool wallet::delete_wallet()
{
    load_stored_wallet();

    if (_state != state::open)
    {
        on_error("[delete_wallet] Wallet not open");
        return false;
    }

    _master_password.clear();
    _items.clear();
    _state = state::not_loaded;
    update_stored_wallet();
    return true;
}

bool wallet::check_password(const password_t& mp_)
{
    if (_state == state::not_loaded)
    {
        on_error("[check_password] No wallet loaded");
        return false;
    }

    if (mp_ == _master_password)
    {
        _state = state::open;
        return true;
    }
    return false;
}

bool wallet::change_master_password(const password_t& old_mp_, const password_t& new_mp_)
{
    if (_state == state::not_loaded)
    {
        on_error("[change_master_password] No wallet loaded");
        return false;
    }

    if (!check_password(old_mp_))
    {
        on_error("[change_master_password] Invalid master password");
        return false;
    }
    _master_password = new_mp_;
    _state           = state::open;
    update_stored_wallet();
    return true;
}

bool wallet::add_item(const item_t& item_)
{
    if (_state != state::open)
    {
        on_error("[add_item] Wallet not open");
        return false;
    }

    const auto it = std::find_if(_items.begin(), _items.end(), [id = item_.id](const auto& i_) { return i_.id == id; });
    if (it != _items.end())
    {
        on_error("[add_item] Item with given ID already exists");
        return false;
    }
    _items.insert(item_);
    update_stored_wallet();
    return true;
}

bool wallet::delete_item(const id_t& id_)
{
    if (_state != state::open)
    {
        on_error("[delete_item] Wallet not open");
        return false;
    }

    const auto it = std::find_if(_items.begin(), _items.end(), [&id_](const auto& i_) { return i_.id == id_; });
    if (it == _items.end())
    {
        on_error("[delete_item] Item with given ID doesn't exist");
        return false;
    }
    _items.erase(it);
    update_stored_wallet();
    return true;
}

bool wallet::show_item(const id_t& id_, login_t& login_, password_t& password_) const
{
    if (_state != state::open)
    {
        on_error("[show_item] Wallet not open");
        return false;
    }

    const auto it = std::find_if(_items.begin(), _items.end(), [&id_](const auto& i_) { return i_.id == id_; });
    if (it == _items.end())
    {
        on_error("[show_item] Item with given ID doesn't exist");
        return false;
    }
    login_    = it->username;
    password_ = it->password;
    return true;
}

std::set<item_t> wallet::show_all_items() const
{
    if (_state != state::open)
    {
        on_error("[show_all_items] Wallet not open");
        return {};
    }
    return _items;
}

std::vector<id_t> wallet::list_all_ids() const
{
    if (_state != state::open)
    {
        on_error("[list_all_ids] Wallet not open");
        return {};
    }

    std::vector<id_t> result;
    std::transform(_items.begin(), _items.end(), std::back_inserter(result), [](const auto& i_) { return i_.id; });
    return result;
}

uint32_t wallet::get_max_field_size() const
{
    return _max_field_size; // This is public constant, no need to check if wallet is open
}

uint32_t wallet::get_ids_total_size() const
{
    if (_state != state::open)
    {
        on_error("[get_ids_total_size] Wallet not open");
        return 0;
    }

    return std::accumulate(_items.begin(), _items.end(), 0ul, [](uint32_t acc_, const auto& i_) { return acc_ + i_.id.size() + 1; });
}

uint32_t wallet::get_wallet_total_size() const
{
    if (_state != state::open)
    {
        on_error("[get_wallet_total_size] Wallet not open");
        return 0;
    }
    const auto acc = [](uint32_t acc_, const auto& i_) { return acc_ + i_.id.size() + i_.username.size() + i_.password.size() + 3; };
    return std::accumulate(_items.begin(), _items.end(), 0ul, acc);
}


void wallet::on_error(const std::string& message_) const
{
    ::on_error(message_.c_str());
}

/********************************************************************************/
/******************************* Private methods ********************************/
/********************************************************************************/

void wallet::update_stored_wallet() const
{
    const auto wallet_size = get_wallet_total_size() + _master_password.size() + 1;
    const auto sealed_size = wallet_size + sizeof(sgx_sealed_data_t);

    std::unique_ptr<uint8_t[]> data(new uint8_t[wallet_size]);
    std::unique_ptr<uint8_t[]> sealed_data(new uint8_t[sealed_size]);
    std::fill_n(data.get(), wallet_size, 0);
    std::fill_n(sealed_data.get(), sealed_size, 0);

    auto* output = data.get();
    output       = std::copy(_master_password.begin(), _master_password.end(), output);
    output++;
    for (const auto& item : _items)
    {
        output = std::copy(item.id.begin(), item.id.end(), output);
        output++;
        output = std::copy(item.username.begin(), item.username.end(), output);
        output++;
        output = std::copy(item.password.begin(), item.password.end(), output);
        output++;
    }

    sgx_status_t sealing_status = seal_wallet(data.get(), wallet_size, (sgx_sealed_data_t*)sealed_data.get(), sealed_size);

    if (sealing_status != SGX_SUCCESS)
    {
        on_error("[update_stored_wallet] Failed to seal wallet");
        return;
    }

    int ret;
    sgx_status_t stored_status = store_wallet(&ret, sealed_data.get(), sealed_size);
    if (ret != 0 || stored_status != SGX_SUCCESS)
    {
        on_error("[update_stored_wallet] Failed to store wallet to file");
    }
}

bool wallet::load_stored_wallet()
{
    size_t sealed_size{};
    const auto status = get_file_size(&sealed_size);
    if (status != SGX_SUCCESS)
    {
        on_error("[load_stored_wallet] Failed to access wallet");
        return false;
    }

    if (sealed_size == 0)
    {
        on_error("[load_stored_wallet] Wallet is not created");
        return false;
    }

    const size_t wallet_size = sealed_size - sizeof(sgx_sealed_data_t);
    uint32_t loaded_size     = wallet_size;
    std::unique_ptr<uint8_t[]> sealed_data(new uint8_t[sealed_size]);
    std::unique_ptr<uint8_t[]> unsealed_data(new uint8_t[wallet_size]);

    int ret;
    const auto load_success = load_wallet(&ret, sealed_data.get(), sealed_size);
    if (ret != 0 || load_success != SGX_SUCCESS)
    {
        on_error("[load_stored_wallet] Failed to load wallet from file");
        return false;
    }
    sgx_status_t sealing_status = unseal_wallet((sgx_sealed_data_t*)sealed_data.get(), unsealed_data.get(), &loaded_size);
    if (sealing_status != SGX_SUCCESS)
    {
        on_error("[load_stored_wallet] Failed to unseal wallet");
        return false;
    }

    std::set<item_t> items;
    const char* it        = (char*)unsealed_data.get();
    const char* const end = (char*)unsealed_data.get() + loaded_size;
    _master_password      = std::string{it};
    it += _master_password.size() + 1;

    while (it < end)
    {
        item_t item;
        item.id = std::string{it};
        it += item.id.size() + 1;
        item.username = std::string{it};
        it += item.username.size() + 1;
        item.password = std::string{it};
        it += item.password.size() + 1;
        items.emplace(std::move(item));
    }
    _items = std::move(items);
    return true;
}
} // namespace wuss
