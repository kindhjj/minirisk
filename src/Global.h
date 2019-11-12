#pragma once

#include <cstddef>
#include <string>

using std::string;
using std::size_t;

namespace minirisk {

extern const string ir_rate_prefix;
extern const string ir_curve_discount_prefix;
extern const string fx_spot_prefix;
extern const string fx_forward_prefix;
extern string base_ccy;
extern unsigned count_port;

inline string ir_curve_discount_name(const string& ccy)
{
    return ir_curve_discount_prefix + ccy;
}

inline string fx_spot_name(const string& ccy1, const string& ccy2)
{
    return fx_spot_prefix + ccy1 + "." + ccy2;
}

inline string fx_forward_name(const string& ccy1, const string& ccy2)
{
    return fx_forward_prefix + ccy1 + "." + ccy2;
}

unsigned get_counting();

string format_label(const string& s);

} // namespace minirisk
