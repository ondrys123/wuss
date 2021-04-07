#include "wallet.hpp"
#include "enclave_t.h"
#include <memory>
#include <unordered_map>
#include <vector>

namespace wuss
{
std::unique_ptr<wallet> wallet::_instance = nullptr;

wallet& wallet::get_instance()
{
    if (!_instance)
        _instance = std::unique_ptr<wallet>(new wallet(wallet::token{}));
    return *_instance;
}

bool wallet::create_wallet(const password_t& /*mp_*/)
{
    //    on_error("create_wallet Not yet implemeted");
    return true;
}

bool wallet::delete_wallet(const password_t& /*mp_*/)
{
    on_error("delete_wallet Not yet implemeted");
    return false;
}

bool wallet::check_password(const password_t& /*mp_*/)
{
    on_error("check_password Not yet implemeted");
    return false;
}

bool wallet::change_master_password(const password_t& /*old_mp_*/, const password_t& /*new_mp_*/)
{
    on_error("change_master_password Not yet implemeted");
    return false;
}

bool wallet::add_item(const item& /*item_*/)
{
    on_error("add_item Not yet implemeted");
    return false;
}

bool wallet::delete_item(const id_t& /*id_*/)
{
    on_error("delete_item Not yet implemeted");
    return false;
}

bool wallet::show_item(const id_t& id_, login_t& login_, password_t& password_)
{
    if (id_ == "test")
    {
        login_    = "User";
        password_ = "dragon1";
        return true;
    }
    return false;
}

std::vector<item> wallet::show_all_items() const
{
    return {};
}

std::vector<id_t> wallet::list_all_ids() const
{
    return {"test"};
}

uint32_t wallet::get_ids_total_size() const
{
    const uint32_t result = std::string("test").size() + 1; // Accumulate total
    return result;
}

uint32_t wallet::get_wallet_total_size() const
{
    const uint32_t result = 512; // Accumulate total
    return result;
}


void wallet::on_error(const std::string& message_) const
{
    ::on_error(message_.c_str());
}

wallet::wallet(wallet::token)
{
    _items.push_back(item{"asd", "asd", "asd"});
}

} // namespace wuss
