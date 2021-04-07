#include "enclave_u.h"
#include "enclave_wrapper.hpp"

/** We implement the ocall functions here **/
using namespace wuss;

void on_error(const char* message_)
{
    enclave_wrapper::get_instance().on_error(message_);
}
