#include "enclave_t.h"
#include "wallet.hpp"

/** We implement the ocall functions here **/
using namespace wuss;

int create_wallet(const char* mp_)
{
    return wallet::get_instance().create_wallet(mp_);
}

int delete_wallet(const char* mp_)
{
    return wallet::get_instance().delete_wallet(mp_);
}

int check_password(const char* mp_)
{
    return wallet::get_instance().check_password(mp_);
}

int change_master_password(const char* old_mp_, const char* new_mp_)
{
    return wallet::get_instance().change_master_password(old_mp_, new_mp_);
}

int add_item(const char* id_, const char* login_, const char* password_)
{
    return wallet::get_instance().add_item(item_t{id_, login_, password_});
}

int delete_item(const char* id_)
{
    return wallet::get_instance().delete_item(id_);
}

uint32_t get_ids_total_size(void)
{
    return wallet::get_instance().get_ids_total_size();
}
uint32_t get_wallet_total_size(void)
{
    return wallet::get_instance().get_wallet_total_size();
}

int show_item(const char* id_, char* output_, uint32_t output_max_size_)
{
    std::string login;
    std::string password;
    auto found = wallet::get_instance().show_item(id_, login, password);
    if (!found)
        return false;

    const auto max_item_size = (output_max_size_ / 2) - 1ul;
    // Copy login
    auto to_copy = std::min(login.size(), max_item_size);
    std::copy_n(login.begin(), to_copy, output_);
    output_ += to_copy + 1;
    // Copy password
    to_copy = std::min(password.size(), max_item_size);
    std::copy_n(password.begin(), to_copy, output_);
    return true;
}

int list_all_ids(char* output_, uint32_t output_max_size_)
{
    const auto& w_inst = wallet::get_instance();
    if (output_max_size_ < w_inst.get_ids_total_size())
    {
        return false;
    }
    const auto ids = w_inst.list_all_ids();
    for (const auto& id : ids)
    {
        std::copy_n(id.begin(), id.size(), output_);
        output_ += id.size() + 1;
    }
    return true;
}

int show_all_items(char* /*output_*/, uint32_t /*output_max_size_*/)
{
    const auto& w_inst = wallet::get_instance();
    w_inst.get_wallet_total_size();
    // TODO
    return false;
}
