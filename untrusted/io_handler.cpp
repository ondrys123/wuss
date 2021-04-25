#include <boost/program_options.hpp>
#include <iostream>
#include <optional>
#include <string>

#include "../trusted/item.hpp"
#include "./enclave_wrapper.hpp"
#include "io_handler.hpp"

namespace wuss
{
int io_handler::run(std::size_t argc, char* argv[])
{
    namespace po = boost::program_options;

    try
    {
        std::optional<action_t> action;

        po::options_description desc("Allowed options");
        // clang-format off
        desc.add_options()
            ("help,h", po::bool_switch()->notifier([&](bool b_){if (b_) {action = action_t::help;}}), "show help message")
            ("new-wallet,w", po::bool_switch()->notifier([&](bool b_){if (b_) {action = action_t::new_wallet;}}), "creates new wallet")
            ("delete-wallet,d", po::bool_switch()->notifier([&](bool b_){if (b_) {action = action_t::delete_wallet;}}), "deletes wallet")
            ("new-entry,n", po::bool_switch()->notifier([&](bool b_){if (b_) {action = action_t::new_entry;}}), "creates new entry")
            ("edit-entry,e", po::bool_switch()->notifier([&](bool b_){if (b_) {action = action_t::edit_entry;}}), "edits entry")
            ("view-entry,v", po::bool_switch()->notifier([&](bool b_){if (b_) {action = action_t::view_entry;}}), "shows entry")
            ("remove-entry,r", po::bool_switch()->notifier([&](bool b_){if (b_) {action = action_t::remove_entry;}}), "removes entry")
            ("view-all-entries,a", po::bool_switch()->notifier([&](bool b_){if (b_) {action = action_t::view_all_entries;}}),"view all entries")
            ("change-master-password,p", po::bool_switch()->notifier([&](bool b_){if (b_) {action = action_t::change_master_password;}}), "changes master password");
        // clang-format on
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
        po::notify(vm);

        const bool too_many_arguments = argc > 2;
        if (!action || action == action_t::help || too_many_arguments)
        {
            if (too_many_arguments)
            {
                std::cerr << "Too many arguments\n";
            }
            io_handler::handle_help(desc);
            return 0;
        }

        switch (*action)
        {
        case action_t::help:
            break;
        case action_t::new_wallet:
            io_handler::handle_create_new_wallet();
            break;
        case action_t::delete_wallet:
            io_handler::handle_delete_wallet();
            break;
        case action_t::new_entry:
            io_handler::handle_new_entry();
            break;
        case action_t::view_entry:
            io_handler::handle_view_entry();
            break;
        case action_t::edit_entry:
            io_handler::handle_edit_entry();
            break;
        case action_t::remove_entry:
            io_handler::handle_remove_entry();
            break;
        case action_t::view_all_entries:
            io_handler::handle_view_all_entries();
            break;
        case action_t::change_master_password:
            io_handler::handle_change_master_password();
            break;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}

void io_handler::handle_help(const po::options_description& description_) noexcept
{
    std::cout << "Usage: options_description [options]\n";
    std::cout << description_;
}

void io_handler::handle_create_new_wallet() noexcept
{
    const std::string mp = io_handler::read_input("Enter new master password: ");
    std::cout << "New master password:" << mp << std::endl;
    enclave_wrapper::get_instance().create_wallet(mp);
}

void io_handler::handle_delete_wallet() noexcept
{
    const std::string mp = io_handler::get_master_password();
    enclave_wrapper::get_instance().delete_wallet();
}

void io_handler::handle_new_entry() noexcept
{
    if (!check_master_password())
    {
        return;
    }

    item_t entry;
    entry.id       = io_handler::get_item_id();
    entry.username = io_handler::get_item_login();
    entry.password = io_handler::get_item_password();
    enclave_wrapper::get_instance().add_item(entry);
}

void io_handler::handle_edit_entry() noexcept
{
    std::cout << "Editing entry is not implemented\n";
}

void io_handler::handle_view_entry() noexcept
{
    if (check_master_password())
    {
        const std::string id = io_handler::get_item_id();
        enclave_wrapper::get_instance().show_item(id);
    }
}

void io_handler::handle_remove_entry() noexcept
{
    if (check_master_password())
    {
        const std::string id = io_handler::get_item_id();
        enclave_wrapper::get_instance().delete_item(id);
    }
}

void io_handler::handle_view_all_entries() noexcept
{
    if (check_master_password())
    {
        //        enclave_wrapper::get_instance().show_all_items();
        for (const auto& v : enclave_wrapper::get_instance().list_all_ids())
        {
            std::cout << "'" << v << "'" << std::endl;
        }
    }
}

void io_handler::handle_change_master_password() noexcept
{
    const std::string mp     = io_handler::get_master_password();
    const std::string new_mp = io_handler::read_input("Enter new master password: ");
    enclave_wrapper::get_instance().change_master_password(mp, new_mp);
}

bool io_handler::check_master_password() noexcept
{
    enclave_wrapper::get_instance().check_password("master");
    return true;
}

std::string io_handler::get_master_password() noexcept
{
    return io_handler::read_input("Enter master password: ");
}

std::string io_handler::get_item_id() noexcept
{
    return io_handler::read_input("Enter id of the entry: ");
}

std::string io_handler::get_item_login() noexcept
{
    return io_handler::read_input("Enter login of the entry: ");
}

std::string io_handler::get_item_password() noexcept
{
    return io_handler::read_input("Enter password of the entry: ");
}

std::string io_handler::read_input(const std::string& output) noexcept
{
    if (!output.empty())
    {
        std::cout << output;
    }

    std::string input;
    std::cin >> input;
    return input;
}
} // namespace wuss
