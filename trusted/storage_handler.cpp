#include "storage_handler.hpp"
#include "sgx_trts.h"
#include "sgx_tseal.h"

sgx_status_t seal_wallet(const wuss::wallet* wallet_, sgx_sealed_data_t* sealed_data, size_t sealed_size )
{
    //return sgx_seal_data(0, NULL, wuss::wallet::get_instance().get_wallet_total_size(), (uint8_t*)wallet_, sealed_size, sealed_data);
    return sgx_seal_data(0, NULL, sizeof(wuss::wallet), (uint8_t*)wallet_, sealed_size, sealed_data);
}

sgx_status_t unseal_wallet(const sgx_sealed_data_t* sealed_data, wuss::wallet* wallet_, uint32_t wallet_size )
{
    return sgx_unseal_data(sealed_data, NULL, NULL, (uint8_t*)&wallet_, &wallet_size);
}