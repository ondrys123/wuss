#pragma once

#include "sgx_tseal.h"
#include "wallet.hpp"

sgx_status_t seal_wallet(const uint8_t* wallet_, size_t wallet_size_, sgx_sealed_data_t* sealed_data_, size_t sealed_size_);

sgx_status_t unseal_wallet(const sgx_sealed_data_t* sealed_data_, uint8_t* wallet_, uint32_t* wallet_size_);