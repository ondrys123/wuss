#pragma once

#include "../trusted/item.hpp"
#include "sgx_error.h"
#include <boost/core/noncopyable.hpp>
#include <iostream>
#include <optional>
#include <sgx_eid.h>
#include <string>
#include <vector>

namespace wuss
{
class enclave_wrapper : private boost::noncopyable
{
private:
    struct token
    {}; // Constructor token

    // Constructors, static methods
public:
    /** @brief Creates enclave **/
    enclave_wrapper(token);

    /**
     * @brief Get singleton instance
     * @return reference to enclave_wrapper instance
     */
    static enclave_wrapper& get_instance();

    /** @brief Destroys enclave **/
    ~enclave_wrapper();

    // Public methods
public:
    /**
     * @brief Returns enclave ID
     * @return encalve ID
     */
    sgx_enclave_id_t get_enclave_id() const { return _eid; }

    /**
     * @brief Creates new wallet
     * @param mp_ master password
     * @return true on success
     */
    bool create_wallet(const password_t& mp_);

    /**
     * @brief Deletes existing wallet
     * @return true on success
     */
    bool delete_wallet();

    /**
     * @brief Checks that given password is same as the one in wallet
     * @param mp_ master password
     * @return true if passwords match
     */
    bool check_password(const password_t& mp_);

    /**
     * @brief Sets new master password for the wallet
     * @param old_mp_ current master password
     * @param new_mp_ new master password
     * @return true if successfully changed
     */
    bool change_master_password(const password_t& old_mp_, const password_t& new_mp_);

    /**
     * @brief Add new item to the wallet
     * @return true on success
     */
    bool add_item(const item_t& item_);

    /**
     * @brief Delete existing item from wallet
     * @return true on success
     */
    bool delete_item(const id_t& id_);

    /**
     * @brief Shows login and password for given id
     * @return true
     */
    std::optional<item_t> show_item(const id_t& id_);

    /**
     * @brief Shows all items that are stored in wallet
     * @return vector of wallet items
     */
    std::vector<item_t> show_all_items();

    /**
     * @brief Shows IDs that are stored in wallet
     * @return vector of IDs
     */
    std::vector<id_t> list_all_ids();

    // OCALL Handlers
public:
    void on_error(const std::string& error_);
    size_t get_file_size();
    int store_wallet(const uint8_t* sealed_data, const size_t sealed_size);
    int load_wallet(uint8_t* sealed_data, const size_t sealed_size);
    int wallet_exists(void);


    // Private methods
private:
    static void throw_on_failure(sgx_status_t status_, const std::string& error_msg_ = "ECALL error");

    // Private members
private:
    static std::optional<enclave_wrapper> _instance; ///< Used enclave_wrapper instance
    sgx_enclave_id_t _eid{};                         ///< Enclave ID
};
} // namespace wuss
