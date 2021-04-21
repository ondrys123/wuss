#pragma once

#include "item.hpp"
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace wuss
{
class wallet
{
private:
    struct token
    {};

    // Constructors, static methods
public:
    /** @brief Creates wallet **/
    wallet(token);

    /**
     * @brief Get singleton instance
     * @return reference to wallet instance
     */
    static wallet& get_instance();

    wallet& operator=(const wallet&) = delete;
    wallet(const wallet&)            = delete;

    // Public methods
public:
    bool create_wallet(const password_t& mp_);
    bool delete_wallet(const password_t& mp_);
    bool check_password(const password_t& mp_);
    bool change_master_password(const password_t& old_mp_, const password_t& new_mp_);
    bool add_item(const item_t& item_);
    bool delete_item(const id_t& id_);
    bool show_item(const id_t& id_, login_t& login_, password_t& password_);
    std::set<item_t> show_all_items() const;
    std::vector<id_t> list_all_ids() const;

    uint32_t get_ids_total_size() const;
    uint32_t get_wallet_total_size() const;

    // OCALL Handlers
public:
    void on_error(const std::string& message_) const;

    // Private members
private:
    static std::unique_ptr<wallet> _instance; ///< Wallet instance

    std::string _master_password; ///< Master password for currently loaded wallet
    std::set<item_t> _items;      ///< Items stored in currently loaded wallet
};
} // namespace wuss
