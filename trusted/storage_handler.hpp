
#include "wallet.hpp"
#include "sgx_trts.h"
#include "sgx_tseal.h"

sgx_status_t seal_wallet(const uint8_t* wallet_, size_t wallet_size, sgx_sealed_data_t* sealed_data, size_t sealed_size );

sgx_status_t unseal_wallet(const sgx_sealed_data_t* sealed_data, uint8_t* wallet_, uint32_t* wallet_size );