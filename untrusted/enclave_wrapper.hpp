#pragma once

#include <optional>
#include <sgx_eid.h>

class enclave_wrapper
{
private:
    struct token
    {};

public: // Constructors, static methods
    enclave_wrapper(token);

    // TODO use boost noncopyable
    enclave_wrapper(const enclave_wrapper&) = delete;
    enclave_wrapper& operator=(const enclave_wrapper&) = delete;

    /**
     * @brief Get singleton instance
     * @return reference to enclave_wrapper instance
     */
    static enclave_wrapper& get_instance();


public: // Public methods
    sgx_enclave_id_t get_enclave_id() const { return _enclave_id; }
    int call_test(int i_);

public: // OCALL Handlers
    void print_int_impl(int i_);

private: // Private members
    static std::optional<enclave_wrapper> _instance;
    sgx_enclave_id_t _enclave_id{};
};
