#include "untrusted/enclave_wrapper.hpp"

#include <iomanip>
#include <iostream>

int main()
{
    try
    {
        auto& w = enclave_wrapper::get_instance();

        const int res = w.call_test(42);
        std::cout << "call_test returned " << res << std::endl;
        return 0;
    }
    catch (const std::runtime_error& e_)
    {
        std::cerr << "Error:" << e_.what() << std::endl;
    }
}
