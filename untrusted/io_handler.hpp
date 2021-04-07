#pragma once

#include <boost/program_options.hpp>
#include <string>

#include "../trusted/item.hpp"
#include "./enclave_wrapper.hpp"

namespace wuss
{
namespace po = boost::program_options;

enum class action_t
{
    help,
    new_wallet,
    delete_wallet,
    new_entry,
    edit_entry,
    view_entry,
    remove_entry,
    view_all_entries,
    change_master_password
};

class io_handler
{
public:
    static int run(std::size_t argc, char* argv[]);

private:
    static void handle_help(const po::options_description& description_) noexcept;
    static void handle_create_new_wallet() noexcept;
    static void handle_delete_wallet() noexcept;
    static void handle_new_entry() noexcept;
    static void handle_edit_entry() noexcept;
    static void handle_view_entry() noexcept;
    static void handle_remove_entry() noexcept;
    static void handle_view_all_entries() noexcept;
    static void handle_change_master_password() noexcept;
    static bool check_master_password() noexcept;

    static std::string get_master_password() noexcept;
    static std::string get_item_id() noexcept;
    static std::string get_item_login() noexcept;
    static std::string get_item_password() noexcept;

    static std::string read_input(const std::string& output = "") noexcept;
};
} // namespace wuss
