#include "storage_handler.hpp"
#include "sgx_trts.h"
#include "sgx_tseal.h"

sgx_status_t seal_wallet(const uint8_t* wallet_, size_t wallet_size, sgx_sealed_data_t* sealed_data, size_t sealed_size )
{
    return sgx_seal_data(0, NULL, wallet_size, (uint8_t*)wallet_, sealed_size, sealed_data);
}

sgx_status_t unseal_wallet(const sgx_sealed_data_t* sealed_data, uint8_t* wallet_, uint32_t wallet_size )
{
    return sgx_unseal_data(sealed_data, NULL, NULL, (uint8_t*)&wallet_, &wallet_size);
}