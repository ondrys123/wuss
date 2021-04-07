#include "untrusted/enclave_wrapper.hpp"

#include <boost/program_options.hpp>
#include <iomanip>
#include <iostream>

namespace po = boost::program_options;
using namespace wuss;
int main()
{
    try
    {
        po::options_description options("TODO");
        options.add_options()("help,h", "prints help");
        auto& w = enclave_wrapper::get_instance();
        w.create_wallet("dragon1");
        for (const auto& id : w.list_all_ids())
        {
            std::cout << "ID: " << id << std::endl;
        }
    }
    catch (const std::runtime_error& e_)
    {
        std::cerr << "Error:" << e_.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
