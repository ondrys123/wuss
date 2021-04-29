#include "storage_handler.hpp"
#include "sgx_tseal.h"

sgx_status_t seal_wallet(const uint8_t* wallet_, size_t wallet_size_, sgx_sealed_data_t* sealed_data_, size_t sealed_size_)
{
    return sgx_seal_data(0, NULL, wallet_size_, wallet_, sealed_size_, sealed_data_);
}

sgx_status_t unseal_wallet(const sgx_sealed_data_t* sealed_data_, uint8_t* wallet_, uint32_t* wallet_size_)
{
    return sgx_unseal_data(sealed_data_, NULL, NULL, wallet_, wallet_size_);
}