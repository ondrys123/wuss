#include "wallet.hpp"
#include "enclave_t.h"
#include <memory>
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

bool wallet::create_wallet(const password_t& mp_)
{
    print_error_message("MP = " + mp_);
    print_error_message("create_wallet Not yet implemeted");
    return false;
}

bool wallet::delete_wallet(const password_t& /*mp_*/)
{
    print_error_message("delete_wallet Not yet implemeted");
    return false;
}

bool wallet::check_password(const password_t& /*mp_*/)
{
    print_error_message("check_password Not yet implemeted");
    return false;
}

bool wallet::change_master_password(const password_t& /*old_mp_*/, const password_t& /*new_mp_*/)
{
    print_error_message("change_master_password Not yet implemeted");
    return false;
}


void wallet::print_error_message(const std::string& message_)
{
    ::print_error_message(message_.c_str());
}

wallet::wallet(wallet::token)
{
    _items.push_back(item{"asd", "asd", "asd"});
}

} // namespace wuss
