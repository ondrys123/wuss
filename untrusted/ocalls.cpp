#include "enclave_u.h"
#include "enclave_wrapper.hpp"

/** We implement the ocall functions here **/

void print_int(int i_)
{
    enclave_wrapper::get_instance().print_int_impl(i_);
}
