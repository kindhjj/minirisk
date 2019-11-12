#include "Global.h"
#include <iomanip>
#include <sstream>

namespace minirisk {

const string ir_rate_prefix = "IR.";
const string ir_curve_discount_prefix = "IR.DISCOUNT.";
const string fx_spot_prefix = "FX.SPOT.";
const string fx_forward_prefix = "FX.FORWARD.";
string base_ccy = "USD";
unsigned count_port = 0;

string format_label(const string& s)
{
    std::ostringstream os;
    os << std::setw(20) << std::left << s;
    return os.str();
}

unsigned get_counting()
{
    count_port++;
    return (count_port - 1);
}
}
