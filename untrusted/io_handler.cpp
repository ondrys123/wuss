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
            ("view-all-ids,i", po::bool_switch()->notifier([&](bool b_){if (b_) {action = action_t::view_all_ids;}}),"view all ids")
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
                std::cout << "Too many arguments\n";
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
        case action_t::view_all_ids:
            io_handler::handle_view_all_ids();
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
        std::cout << e.what() << "\n";
        return 1;
    }
    return 0;
}

void io_handler::handle_help(const po::options_description& description_)
{
    std::cout << "Usage: options_description [options]\n";
    std::cout << description_;
}

void io_handler::handle_create_new_wallet()
{
    const std::string mp = io_handler::read_input("Enter new master password: ");
    if (enclave_wrapper::get_instance().create_wallet(mp)) 
    {
        std::cout << "New wallet created with master password: " << mp << "\n";
    } 
    else
    {
        std::cout << "Failed to create new wallet\n";
    }
}

void io_handler::handle_delete_wallet()
{
    if (!check_master_password())
    {
        std::cout << "Incorrect password\n";
        return;
    }

    if (enclave_wrapper::get_instance().delete_wallet()) 
    {
        std::cout << "Wallet successfully deleted\n";
    }
    else
    {
        std::cout << "Failed to delete wallet\n";
    }
}

void io_handler::handle_new_entry()
{
    if (!check_master_password())
    {
        std::cout << "Incorrect password\n";
        return;
    }

    item_t entry;
    entry.id       = io_handler::get_item_id();
    entry.username = io_handler::get_item_login();
    entry.password = io_handler::get_item_password();
    enclave_wrapper::get_instance().add_item(entry);
}

void io_handler::handle_edit_entry()
{
    const auto gn = [](std::string type, std::string old_value){
        const auto change = io_handler::read_input("Do you want to change value of " + type + "? (y/n): ");
        if (change == "n") 
        {
            return old_value;
        }

        return io_handler::read_input("Enter new value of " + type + ": ");
    };

    if (!check_master_password()) 
    {
        std::cout << "Incorrect password\n";
        return;
    }

    const std::string id = io_handler::get_item_id();
    const auto old_item = enclave_wrapper::get_instance().show_item(id);
    if (!old_item)
    {
        std::cout << "Entry with given id not found\n";
        return;
    }

    item_t new_item;
    new_item.id = gn("item", old_item->id);
    new_item.username = gn("username", old_item->username);
    new_item.password = gn("password", old_item->password);

    if (!enclave_wrapper::get_instance().delete_item(id)) 
    {
        std::cout << "Failed to edit entry\n";
        return;
    }

    if (!enclave_wrapper::get_instance().add_item(new_item)) 
    {
        std::cout << "Failed to edit entry\n";
    }
}

void io_handler::handle_view_entry()
{
    if (!check_master_password())
    {
        std::cout << "Incorrect password\n";
        return;
    }

    const std::string id = io_handler::get_item_id();
    const auto& item = enclave_wrapper::get_instance().show_item(id);
    if (!item) 
    {
        std::cout << "Failed to show entry\n";
        return;
    }

    std::cout << "Username: " << item->username << "\n";
    std::cout << "Password: " << item->password << "\n";
}

void io_handler::handle_remove_entry()
{
    if(!check_master_password())
    {
        std::cout << "Incorrect password\n";
        return;
    }

    const std::string id = io_handler::get_item_id();
    if (enclave_wrapper::get_instance().delete_item(id)) 
    {
        std::cout << "Entry \"" << id << "\" successfully removed\n";
    }
    else 
    {
        std::cout << "Failed to remove entry\n";
    }
}

void io_handler::handle_view_all_ids()
{
    if (!check_master_password())
    {
        std::cout << "Incorrect password\n";
        return;
    }
    
    const auto& ids = enclave_wrapper::get_instance().list_all_ids();
    if (ids.empty()) {
        std::cout << "No entries found\n";
        return;
    }
    
    std::cout << "===================================";
    for (const auto& id : ids)
    {
        std::cout << id << "\n";
    }
    std::cout << "===================================";
}

void io_handler::handle_view_all_entries()
{
    if (!check_master_password())
    {
        std::cout << "Wrong master password\n";
        return;
    }

    const auto& entries = enclave_wrapper::get_instance().show_all_items();
    if (entries.empty())
    {
        std::cout << "No entries found\n";
        return;
    }

    std::cout << "Listing all entries:\n";
    std::cout << "===================================\n";
    for (const auto& item : entries)
    {
        std::cout << "Id: " << item.id << "\n";
        std::cout << "Username: " << item.username << "\n";
        std::cout << "Password: " << item.password << "\n";
        std::cout << "===================================\n";
    }
}

void io_handler::handle_change_master_password()
{
    const std::string mp     = io_handler::get_master_password();
    const std::string new_mp = io_handler::read_input("Enter new master password: ");
    enclave_wrapper::get_instance().change_master_password(mp, new_mp);
}

std::string io_handler::get_master_password()
{
    return io_handler::read_input("Enter master password: ");
}

bool io_handler::check_master_password()
{
    const auto& password = io_handler::get_master_password();
    return enclave_wrapper::get_instance().check_password(password);
}

std::string io_handler::get_item_id()
{
    return io_handler::read_input("Enter id of the entry: ");
}

std::string io_handler::get_item_login()
{
    return io_handler::read_input("Enter login of the entry: ");
}

std::string io_handler::get_item_password()
{
    return io_handler::read_input("Enter password of the entry: ");
}

std::string io_handler::read_input(const std::string& output)
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
