#include <boost/process.hpp>
#include <catch2/catch.hpp>
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
};

run_result run_wuss(const params_t& params_, const params_t& input_ = {})
{
    std::string param =
        std::accumulate(params_.begin(), params_.end(), std::string{}, [](const auto& acc_, const auto& s_) { return acc_ + "\"" + s_ + "\" "; });
    bp::ipstream ips_out;
    bp::ipstream ips_err;
    bp::opstream ops_in;

    bp::child c("wuss", param, bp::std_out > ips_out, bp::std_err > ips_err, bp::std_in < ops_in);
    for (const auto& i : input_)
    {
        ops_in << i;
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


TEST_CASE("wuss help")
{
    auto r = run_wuss({"-h"});
    r.no_err().out_has("--help").out_has("-h").out_has("show help message");
}
