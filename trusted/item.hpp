#pragma once
#include <string>
namespace wuss
{
using id_t       = std::string;
using login_t    = std::string;
using password_t = std::string;

struct item_t
{
    id_t id;
    login_t username;
    password_t password;
};

} // namespace wuss
