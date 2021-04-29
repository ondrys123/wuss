#include "enclave_t.h"
#include "wallet.hpp"

/** We implement the ecall functions here **/
using namespace wuss;

int create_wallet(const char* mp_)
{
    return wallet::get_instance().create_wallet(mp_);
}

int delete_wallet()
{
    return wallet::get_instance().delete_wallet();
}

int check_password(const char* mp_)
{
    return wallet::get_instance().check_password(mp_);
}

int check_password_policy(const char* mp_)
{
    return wallet::get_instance().check_password_policy(mp_);
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

uint32_t get_max_field_size(void)
{
    return wallet::get_instance().get_max_field_size();
}

int show_item(const char* id_, char* output_, uint32_t output_max_size_)
{
    std::string login;
    std::string password;
    auto found = wallet::get_instance().show_item(id_, login, password);
    if (!found)
        return false;
    if (login.size() + password.size() + 2 > output_max_size_)
        return false;

    output_ = std::copy(login.begin(), login.end(), output_);
    output_++;
    std::copy(password.begin(), password.end(), output_);
    return true;
}

int list_all_ids(char* output_, uint32_t output_max_size_)
{
    const auto& w_inst = wallet::get_instance();
    if (output_max_size_ < w_inst.get_ids_total_size())
        return false;

    const auto ids = w_inst.list_all_ids();
    for (const auto& id : ids)
    {
        output_ = std::copy(id.begin(), id.end(), output_);
        output_++;
    }
    return true;
}

int show_all_items(char* output_, uint32_t output_max_size_)
{
    const auto& w_inst = wallet::get_instance();
    if (output_max_size_ < w_inst.get_wallet_total_size())
        return false;

    const auto all_items = w_inst.show_all_items();
    for (const item_t& item : all_items)
    {
        output_ = std::copy(item.id.begin(), item.id.end(), output_);
        output_++;
        output_ = std::copy(item.username.begin(), item.username.end(), output_);
        output_++;
        output_ = std::copy(item.password.begin(), item.password.end(), output_);
        output_++;
    }
    return true;
}
