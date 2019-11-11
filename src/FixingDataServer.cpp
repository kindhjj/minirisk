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
    std::string name;
    std::string date;
    double value;
    do
    {
        is >> name >> date >> value;
        m_data.emplace(name, std::map<Date, double>());
        auto ins = m_data[name].emplace(Date(static_cast<unsigned>(std::stoi(date.substr(0,4))),static_cast<unsigned>(std::stoi(date.substr(4,2))),static_cast<unsigned>(std::stoi(date.substr(6,2)))), value);
        MYASSERT(ins.second, "Duplicated fixing: " << date << " " << value);
    } while (is);
}

    double FixingDataServer::get(const string &name, const Date &t) const
    {

    }

    std::pair<double, bool> FixingDataServer::lookup(const string &name, const Date &t) const
    {

    }

} // namespace minirisk