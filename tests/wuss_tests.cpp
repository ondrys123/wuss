#include <boost/process.hpp>
#include <catch2/catch.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>

namespace bp   = boost::process;
using params_t = std::vector<std::string>;

struct run_result
{
    std::string std_out;
    std::string std_err;
    int ret_code{};
    const run_result& no_cerr() const
    {
        INFO(std_err);
        CHECK(ret_code == 0);
        CHECK(std_err.empty());
        return *this;
    }
    const run_result& out_has(const std::string& s_) const
    {
        INFO("### Output ###\n" << std_out);
        INFO("### Should contain ###\n" << s_);
        CHECK(Catch::contains(std_out, s_));
        return *this;
    }
    const run_result& wallet_exists() const
    {
        CHECK(std::filesystem::is_regular_file("wallet.seal"));
        return *this;
    }
};

run_result run_wuss(const params_t& params_, const params_t& input_ = {})
{
    bp::ipstream ips_out;
    bp::ipstream ips_err;
    bp::opstream ops_in;

    bp::child c("wuss", bp::args(params_), bp::std_out > ips_out, bp::std_err > ips_err, bp::std_in < ops_in);

    for (const auto& i : input_)
    {
        ops_in << i << std::endl;
    }
    run_result res;
    std::string line;
    //    if (c.running())
    {
        while (std::getline(ips_out, line) && !line.empty())
        {
            res.std_out += line + "\n";
        }
        while (std::getline(ips_err, line) && !line.empty())
        {
            res.std_err += line + "\n";
        }
    }
    c.wait();
    res.ret_code = c.exit_code();
    return res;
}

const std::string test_mp = "P@$$W0rd322";

inline void clean_wallet_file()
{
    if (std::filesystem::exists("wallet.seal"))
    {
        std::filesystem::remove("wallet.seal");
    }
}

SCENARIO("Print help and exit")
{
    auto r = run_wuss({"-h"});
    r.no_cerr().out_has("--help").out_has("-h").out_has("show help message");
}

SCENARIO("Create new wallet and add items to it")
{
    GIVEN("Empty wallet")
    {
        clean_wallet_file();
        const auto empty_w = run_wuss({"-w"}, {test_mp});
        THEN("No errors occured when creating an empty wallet") { empty_w.no_cerr().wallet_exists(); }

        WHEN("We print all IDs before adding anything")
        {
            const auto r = run_wuss({"-i"}, {test_mp});
            THEN("Output says wallet is empty") { r.no_cerr().out_has("No entries found"); }
        }

        WHEN("We try to print any item before adding anything")
        {
            const auto r = run_wuss({"-v"}, {test_mp, "facebook"});
            THEN("Output says wallet is empty") { r.no_cerr().out_has("Error:"); }
        }

        WHEN("We print all items before adding anything")
        {
            const auto r = run_wuss({"-a"}, {test_mp});
            THEN("Output says wallet is empty") { r.no_cerr().out_has("No entries found"); }
        }

        WHEN("We add some items")
        {
            const auto add1 = run_wuss({"-n"}, {test_mp, "facebook", "my@mail.com", "$stronkPW"});
            const auto add2 = run_wuss({"-n"}, {test_mp, "minecraft", "123123", "dragon1"});
            THEN("No error occurs")
            {
                add1.no_cerr();
                add1.no_cerr();
            }
            WHEN("We print all IDs")
            {
                const auto r = run_wuss({"-i"}, {test_mp});
                THEN("Output lists our IDs") { r.no_cerr().out_has("facebook").out_has("minecraft"); }
            }

            WHEN("We print all items")
            {
                const auto r = run_wuss({"-a"}, {test_mp});
                THEN("Output lists all items")
                {
                    r.no_cerr();
                    r.out_has("facebook").out_has("my@mail.com").out_has("$stronkPW");
                    r.out_has("minecraft").out_has("123123").out_has("dragon1");
                }
            }
            WHEN("We print one item")
            {
                const auto r = run_wuss({"-v"}, {test_mp, "facebook"});
                THEN("Output says wallet is empty") { r.no_cerr().out_has("my@mail.com").out_has("$stronkPW"); }
            }
            WHEN("We print non-existent item")
            {
                const auto r = run_wuss({"-v"}, {test_mp, "github"});
                THEN("We still get an error") { r.no_cerr().out_has("Error:"); }
            }
        }
    }
    clean_wallet_file();
}

SCENARIO("Create new wallet when one already exists")
{
    GIVEN("Empty wallet")
    {
        clean_wallet_file();
        const auto empty_w = run_wuss({"-w"}, {test_mp});
        THEN("No errors occured when creating an empty wallet") { empty_w.no_cerr().wallet_exists(); }
        WHEN("We try to create new wallet")
        {
            const auto r = run_wuss({"-w"}, {test_mp});
            THEN("Error message is produced") { r.no_cerr().out_has("Error:"); }
        }
    }
    clean_wallet_file();
}

SCENARIO("Operations with non-empty wallet")
{
    GIVEN("Wallet with item")
    {
        clean_wallet_file();
        auto r = run_wuss({"-w"}, {test_mp});
        r.no_cerr().wallet_exists();
        r = run_wuss({"-n"}, {test_mp, "facebook", "my@mail.com", "$stronkPW"});
        r.no_cerr();
        r = run_wuss({"-v"}, {test_mp, "facebook"});
        r.no_cerr().out_has("Username").out_has("my@mail.com").out_has("Password").out_has("$stronkPW");

        WHEN("We try to change master password")
        {
            const std::string new_mp = "N3wM@ster";
            WHEN("Old master password is incorrect")
            {
                r = run_wuss({"-p"}, {"Wrong", new_mp});
                THEN("Error message is produced") { r.no_cerr().out_has("Error:"); }
                THEN("Master password stays the same")
                {
                    r = run_wuss({"-v"}, {test_mp, "facebook"});
                    r.no_cerr().out_has("Username").out_has("my@mail.com").out_has("Password").out_has("$stronkPW");
                }
                THEN("New password doesn't work")
                {
                    r = run_wuss({"-v"}, {new_mp, "facebook"});
                    CHECK(!Catch::contains(r.std_out, "$stronkPW"));
                }
            }
            WHEN("Old master password is correct")
            {
                r = run_wuss({"-p"}, {test_mp, new_mp});
                r.no_cerr();
                THEN("Master password got changed")
                {
                    r = run_wuss({"-v"}, {new_mp, "facebook"});
                    r.no_cerr().out_has("Username").out_has("my@mail.com").out_has("Password").out_has("$stronkPW");
                }
                THEN("Old password doesn't work")
                {
                    r = run_wuss({"-v"}, {test_mp, "facebook"});
                    CHECK(!Catch::contains(r.std_out, "$stronkPW"));
                }
            }
        }
    }
    clean_wallet_file();
}

SCENARIO("Deleting existing wallet")
{
    GIVEN("Wallet with file")
    {
        clean_wallet_file();
        auto r = run_wuss({"-w"}, {test_mp});
        r.no_cerr().wallet_exists();
        WHEN("We try to delete it with wrong password")
        {
            r = run_wuss({"-d"}, {"Wrong"});
            THEN("Error is printed") { r.no_cerr().out_has("Incorrect password"); }
            THEN("File stays") { r.wallet_exists(); }
        }
        WHEN("We try to delete with correct password")
        {
            r = run_wuss({"-d"}, {test_mp});
            THEN("File is gone") { CHECK(!std::filesystem::is_regular_file("wallet.seal")); }
        }
    }
}
