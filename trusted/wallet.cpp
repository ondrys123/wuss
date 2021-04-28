#include "wallet.hpp"
#include "storage_handler.hpp"
#include "enclave_t.h"
#include <numeric>

namespace wuss
{
std::unique_ptr<wallet> wallet::_instance = nullptr;

wallet::wallet(wallet::token)
    : _state{state::not_loaded}
{
    // TODO: try load, if success, set state to loaded

    // Test wallet, remove later:
    /*_state           = state::loaded;
    _master_password = "master";
    _items.insert(item_t{"discord", "dude", "12345"});
    _items.insert(item_t{"Facebook", "my.name@domain.com", "@ C0mpleX Pa$$word"});
    _items.insert(item_t{"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
                         "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
                         "..............................................................................................................................."});
    _items.insert(item_t{"bank account", "9865 3223", "6666"}); 
    update_stored_wallet(*_instance);
    */

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
    int ret;
    sgx_status_t status = wallet_exists(&ret);
    if (ret !=0) {
         on_error("wallet already exists");
    }

    /*if (_state != state::not_loaded)
    {
        on_error("[create_wallet] wallet is already loaded");
        return false;
    }
*/
    _master_password = mp_;
    _items.clear();
    _state = state::open;
    update_stored_wallet( *_instance );
    return true;
}

bool wallet::delete_wallet()
{
    wallet& wal = load_stored_wallet();

    /*if (_state != state::open)
    {
        on_error("[delete_wallet] Wallet not open");
        return false;
    }*/

    _master_password.clear();
    _items.clear();
    _state = state::not_loaded;
    //update_stored_wallet();
    return true;
}

bool wallet::check_password(const password_t& mp_)
{
    /*if (_state == state::not_loaded)
    {
        on_error("[check_password] No wallet loaded");
        return false;
    }*/

    if (mp_ == _master_password)
    {
        _state = state::open;
        return true;
    }
    return false;
}

bool wallet::change_master_password(const password_t& old_mp_, const password_t& new_mp_)
{

    /*if (_state == state::not_loaded)
    {
        on_error("[change_master_password] No wallet loaded");
        return false;
    } */
    wallet& wal = load_stored_wallet();

    if (!check_password(old_mp_))
    {
        on_error("[change_master_password] Invalid master password");
        return false;
    }
    _master_password = new_mp_;
    _state           = state::open;
    update_stored_wallet( *_instance);
    return true;
}

bool wallet::add_item(const item_t& item_)
{
    /*if (_state != state::open)
    {
        on_error("[add_item] Wallet not open");
        return false;
    }*/
    load_stored_wallet();

    const auto it = std::find_if(_items.begin(), _items.end(), [id = item_.id](const auto& i_) { return i_.id == id; });
    if (it != _items.end())
    {
        on_error("[add_item] Item with given ID already exists");
        return false;
    }
    _items.insert(item_);
    update_stored_wallet( *_instance);
    return true;
}

bool wallet::delete_item(const id_t& id_)
{
    /*if (_state != state::open)
    {
        on_error("[delete_item] Wallet not open");
        return false;
    }*/

    const auto it = std::find_if(_items.begin(), _items.end(), [&id_](const auto& i_) { return i_.id == id_; });
    if (it == _items.end())
    {
        on_error("[delete_item] Item with given ID doesn't exist");
        return false;
    }
    _items.erase(it);
    //update_stored_wallet();
    return true;
}

bool wallet::show_item(const id_t& id_, login_t& login_, password_t& password_) const
{
    /*if (_state != state::open)
    {
        on_error("[show_item] Wallet not open");
        return false;
    }
    */
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
   /* if (_state != state::open)
    {
        on_error("[show_all_items] Wallet not open");
        return {};
    }*/

    return _items;
}

std::vector<id_t> wallet::list_all_ids() const
{
    load_stored_wallet(); 
    /*if (_state != state::open)
    {
        on_error("[list_all_ids] Wallet not open");
        return {};
    }*/

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
   /* if (_state != state::open)
    {
        on_error("[get_ids_total_size] Wallet not open");
        return 0;
    }*/

    return std::accumulate(_items.begin(), _items.end(), 0ul, [](uint32_t acc_, const auto& i_) { return acc_ + i_.id.size() + 1; });
}

uint32_t wallet::get_wallet_total_size() const
{
    /*if (_state != state::open)
    {
        on_error("[get_wallet_total_size] Wallet not open");
        return 0;
    }*/
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

void wallet::update_stored_wallet(wallet& wal) const
{
    // TODO Save new stuff to file
    size_t sealed_size = sizeof(sgx_sealed_data_t) + sizeof(wallet);
    uint8_t* sealed_data = (uint8_t*)malloc(sealed_size);
    sgx_status_t sealing_status = seal_wallet(&wal, (sgx_sealed_data_t*)sealed_data, sealed_size);

    if (sealing_status != SGX_SUCCESS){
        free(sealed_data);
        on_error("[update_stored_wallet] Wallet not sealed");
    }
    /*if (sealing_status == SGX_SUCCESS){
        on_error("[update_stored_wallet] Wallet sealed");
    }*/

    int ret;
    sgx_status_t stored_status = store_wallet(&ret, sealed_data, sealed_size);
    free(sealed_data);
    if (ret != 0 || stored_status != SGX_SUCCESS){
        on_error("[update_stored_wallet] Wallet not stored to file");
    }  
}

wallet& wallet::load_stored_wallet() const
    {
    uint32_t wallet_size = sizeof(wallet);
    size_t sealed_size = sizeof(sgx_sealed_data_t) + wallet_size;
    uint8_t* sealed_data = (uint8_t*)malloc(sealed_size);
    int ret;

    bool stored_status = load_wallet(&ret, sealed_data, sealed_size);
    if (ret != 0 || stored_status != SGX_SUCCESS){
        free(sealed_data);
        on_error("[load_stored_wallet] Wallet not loaded from file");
    }

    wallet& wal = get_instance();

    sgx_status_t sealing_status = unseal_wallet((sgx_sealed_data_t*)sealed_data, &wal, wallet_size);
    if (sealing_status != SGX_SUCCESS){
        free(sealed_data);
        on_error("[load_stored_wallet] Wallet not unsealed");
    }
    return wal;

    } 

} // namespace wuss
