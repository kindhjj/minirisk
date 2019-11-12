#include "FixingDataServer.h"

#include <fstream>

#include "Macros.h"
#include "Global.h"

namespace minirisk
{

FixingDataServer::FixingDataServer(const std::string &filename)
{
    std::ifstream is(filename);
    MYASSERT(!is.fail(), "Could not open file " << filename);
    double value;
    do
    {
        std::string name;
        std::string date;
        is >> name >> date >> value;
        if (!name.empty())
        {
            m_data.emplace(name, std::map<unsigned, double>());
            unsigned date_serial = Date(static_cast<unsigned>(std::stoi(date.substr(0, 4))), static_cast<unsigned>(std::stoi(date.substr(4, 2))), static_cast<unsigned>(std::stoi(date.substr(6, 2)))).get_serial();
            auto ins = m_data[name].emplace(date_serial, value);
            MYASSERT(ins.second, "Duplicated fixing: " << date << " " << value);
        }
    } while (is);
}

double FixingDataServer::get(const string &name, const Date &t) const
{
    auto spot_fixing = m_data.find(name);
    MYASSERT(spot_fixing != m_data.end(), "Fixing not found: " << name << "," << t.to_string());
    auto spot_fixing_date = spot_fixing->second.find(t.get_serial());
    MYASSERT(spot_fixing_date != spot_fixing->second.end(), "Fixing not found: " << name << "," << t.to_string());
    return spot_fixing_date->second;
}

std::pair<double, bool> FixingDataServer::lookup(const string &name, const Date &t) const
{
    auto spot_fixing = m_data.find(name);
    if (spot_fixing == m_data.end())
        return std::make_pair(std::numeric_limits<double>::quiet_NaN(), false);
    auto spot_fixing_date = spot_fixing->second.find(t.get_serial());
    if (spot_fixing_date == spot_fixing->second.end())
        return std::make_pair(std::numeric_limits<double>::quiet_NaN(), false);
    return std::make_pair(spot_fixing_date->second, true);
}

} // namespace minirisk