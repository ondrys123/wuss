#pragma once
#include <string>

namespace wuss
{
struct pswd_params_t
{
    uint32_t alpha_count; ///< slphabet characters count
    uint32_t num_count; ///< number count
    uint32_t symbol_count; ///< special symbol count
};
} // namespace wuss