#pragma once

#include <map>
#include <regex>
#include <vector>
#include "Global.h"
#include "Date.h"

namespace minirisk
{

struct FixingDataServer;
typedef std::shared_ptr<const FixingDataServer> ptr_fds_t;

struct FixingDataServer
{
public:
    FixingDataServer(const string &filename);

    // queries
    // return the xing if available, otherwise trigger an error
    double get(const string &name, const Date &t) const;
    // return the xing if available, NaN otherwise, and set the flag if found
    std::pair<double, bool> lookup(const string &name, const Date &t) const;

private:
    // for simplicity, assumes market data can only have type double
    std::map<std::string, std::map<unsigned, double>> m_data;
};

}   //minirisk
