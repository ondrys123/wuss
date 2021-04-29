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
    const run_result& no_err() const
    {
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
    while (c.running() && std::getline(ips_out, line) && !line.empty())
    {
        res.std_out += line + "\n";
    }
    while (c.running() && std::getline(ips_err, line) && !line.empty())
    {
        res.std_err += line + "\n";
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
    r.no_err().out_has("--help").out_has("-h").out_has("show help message");
}

SCENARIO("Create new wallet")
{
    clean_wallet_file();
    GIVEN("Empty wallet")
    {
        const auto empty_w = run_wuss({"-w"}, {test_mp});
        THEN("No errors occur") { empty_w.no_err().wallet_exists(); }

        WHEN("We print all IDs before adding anything")
        {
            const auto r = run_wuss({"-i"}, {test_mp});
            THEN("Output says wallet is empty") { r.no_err().out_has("No entries found"); }
        }

        WHEN("We print all items before adding anything")
        {
            const auto r = run_wuss({"-a"}, {test_mp});
            THEN("Output says wallet is empty") { r.no_err().out_has("No entries found"); }
        }

        WHEN("We add some items")
        {
            const auto add1 = run_wuss({"-n"}, {test_mp, "facebook", "my@mail.com", "$stronkPW"});
            const auto add2 = run_wuss({"-n"}, {test_mp, "minecraft", "123123", "dragon1"});
            THEN("No error occurs")
            {
                add1.no_err();
                add1.no_err();
            }
            WHEN("We print all IDs")
            {
                const auto r = run_wuss({"-i"}, {test_mp});
                THEN("Output lists our IDs") { r.no_err().out_has("facebook").out_has("minecraft"); }
            }

            WHEN("We print all items")
            {
                const auto r = run_wuss({"-a"}, {test_mp});
                THEN("Output lists all items")
                {
                    r.no_err();
                    r.out_has("facebook").out_has("my@mail.com").out_has("$stronkPW");
                    r.out_has("minecraft").out_has("123123").out_has("dragon1");
                }
            }
        }
    }
    clean_wallet_file();
}
