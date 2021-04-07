#pragma once

#include "item.hpp"
#include <string>
#include <vector>

namespace wuss
{
class wallet
{
private:
    struct token
    {};

public: // Constructors, static methods
    /** @brief Creates wallet **/
    wallet(token);

    /**
     * @brief Get singleton instance
     * @return reference to wallet instance
     */
    static wallet& get_instance();

    wallet& operator=(const wallet&) = delete;
    wallet(const wallet&)            = delete;

public: // Public methods
    bool create_wallet(const password_t& mp_);
    bool delete_wallet(const password_t& mp_);
    bool check_password(const password_t& mp_);
    bool change_master_password(const password_t& old_mp_, const password_t& new_mp_);


public: // OCALL Handlers
    void print_error_message(const std::string& message_);


private: // Private members
    static std::unique_ptr<wallet> _instance;
    std::vector<item> _items;
};
} // namespace wuss
