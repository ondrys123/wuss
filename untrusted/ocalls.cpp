#include "enclave_u.h"
#include "enclave_wrapper.hpp"

/** We implement the ocall functions here **/
using namespace wuss;

void print_error_message(const char* message_)
{
    enclave_wrapper::get_instance().print_error_message(message_);
}
