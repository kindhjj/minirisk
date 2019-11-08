#include "MarketDataServer.h"
#include "Macros.h"
#include "Streamer.h"

#include <limits>

namespace minirisk {

// transforms FX.SPOT.EUR.USD into FX.SPOT.EUR
string mds_spot_name(const string& name)
{
    // NOTE: in a real system error checks should be stricter, not just on the last 3 characters
    MYASSERT((name.substr(name.length() - 3, 3) == "USD"),
        "Only FX pairs in the format FX.SPOT.CCY.USD can be queried. Got " << name);
    return name.substr(0, name.length() - 4);
}

MarketDataServer::MarketDataServer(const string& filename)  //read names and values from risk_factor_file
{
    std::ifstream is(filename);
    MYASSERT(!is.fail(), "Could not open file " << filename);
    do {
        string name;
        double value;
        is >> name >> value;
        //std::cout << name << " " << value << "\n";
        auto ins = m_data.emplace(name, value);
        MYASSERT(ins.second, "Duplicated risk factor: " << name);
    } while (is);
}

double MarketDataServer::get(const string& name) const
{
    auto iter = m_data.find(name);
    MYASSERT(iter != m_data.end(), "Market data not found: " << name);
    return iter->second;
}

std::pair<double, bool> MarketDataServer::lookup(const string& name) const
{
    auto iter = m_data.find(name);
    return (iter != m_data.end())  // found?
            ? std::make_pair(iter->second, true)
            : std::make_pair(std::numeric_limits<double>::quiet_NaN(), false);
}


//fetch all risk factors of the corresponding currency to a vector
std::vector<string> MarketDataServer::get_ir_vector(const string& ccy) const{
    std::regex full("IR\\..*" + ccy);
    std::smatch full_matched;
    std::vector<string> matched_v;
    matched_v.reserve(m_data.size());
    for (auto tr : m_data)
    {
        bool if_full_match = std::regex_match(tr.first, full_matched, full);
        if (if_full_match)
        {
            string matched_s(full_matched[0]);
            matched_v.emplace_back(matched_s.substr(3));
        }
    }
    return matched_v;
}

} // namespace minirisk

