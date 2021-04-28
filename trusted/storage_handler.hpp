
#include "wallet.hpp"
#include "sgx_trts.h"
#include "sgx_tseal.h"

sgx_status_t seal_wallet(const wuss::wallet* wallet_, sgx_sealed_data_t* sealed_data, size_t sealed_size );

sgx_status_t unseal_wallet(const sgx_sealed_data_t* sealed_data, wuss::wallet* wallet_, uint32_t wallet_size );