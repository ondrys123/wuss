#pragma once

#include "../trusted/item.hpp"
#include <boost/core/noncopyable.hpp>
#include <optional>
#include <sgx_eid.h>
#include <string>

namespace wuss
{
class enclave_wrapper : private boost::noncopyable
{
private:
    struct token
    {};

public: // Constructors, static methods
    /** @brief Creates enclave **/
    enclave_wrapper(token);

    /**
     * @brief Get singleton instance
     * @return reference to enclave_wrapper instance
     */
    static enclave_wrapper& get_instance();

    /** @brief Destroys enclave **/
    ~enclave_wrapper();

public: // Public methods
    sgx_enclave_id_t get_enclave_id() const { return _enclave_id; }

    bool create_wallet(const std::string mp_);
    bool delete_wallet(const std::string mp_);
    bool change_master_password(const std::string old_mp_, const std::string new_mp_);
    bool add_item(const item&);
    bool delete_item(const id_t&);
    bool show_item(const id_t&);
    bool show_all_items();


public: // OCALL Handlers
    void print_int_impl(int i_);

private: // Private members
    static std::optional<enclave_wrapper> _instance;
    sgx_enclave_id_t _enclave_id{};
};
} // namespace wuss
