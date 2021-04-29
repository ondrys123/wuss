#include "enclave_u.h"
#include "enclave_wrapper.hpp"

/** We implement the ocall functions here **/
using namespace wuss;

void on_error(const char* message_)
{
    enclave_wrapper::get_instance().on_error(message_);
}

size_t get_file_size()
{
    return enclave_wrapper::get_instance().get_file_size();
}

int store_wallet(const uint8_t* sealed_data, const size_t sealed_size)
{
    return enclave_wrapper::get_instance().store_wallet(sealed_data, sealed_size);
}

int load_wallet(uint8_t* sealed_data, const size_t sealed_size)
{
    return enclave_wrapper::get_instance().load_wallet(sealed_data, sealed_size);
}
