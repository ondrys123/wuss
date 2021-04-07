#include "untrusted/enclave_wrapper.hpp"
#include "untrusted/io_handler.hpp"
#include <boost/program_options.hpp>
#include <iomanip>
#include <iostream>


namespace po = boost::program_options;
using namespace wuss;
int main(int argc, char* argv[])
{
    try
    {
        io_handler::run(argc, argv);
    }
    catch (const std::runtime_error& e_)
    {
        std::cerr << "Error:" << e_.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
