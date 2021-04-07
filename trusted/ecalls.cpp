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
