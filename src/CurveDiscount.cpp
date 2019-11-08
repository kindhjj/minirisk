#include "CurveDiscount.h"
#include "Market.h"
#include "Streamer.h"

#include <cmath>


namespace minirisk {

CurveDiscount::CurveDiscount(Market *mkt, const Date& today, const string& curve_name)
    : m_today(today)
    , m_name(ir_rate_prefix + curve_name)    
    , m_rate(mkt->get_yield(curve_name))
{
    m_rate_tenor = m_rate;
    for (auto &rt : m_rate_tenor)
        rt.second *= static_cast<double>(rt.first) / 365.0;
}

double CurveDiscount::df(const Date& m_dt, const unsigned& dt) const
{
    auto lw = m_rate.lower_bound(dt);
    MYASSERT(lw != m_rate.end(), "Curve " << ir_curve_discount_name(m_name.substr(3)) << ", DF not available beyond last tenor date " << Date(m_rate.rbegin()->first + m_today.get_serial()).to_string(true) << ", requested " << m_dt.to_string(true));
    auto up = lw--;
    double up_b = m_rate_tenor.find(up->first)->second;
    double lw_b = m_rate_tenor.find(lw->first)->second;
    double forward_rate = (up_b - lw_b) / (up->first - lw->first);
    return std::exp(-lw_b - forward_rate * (dt - lw->first));
}

void CurveDiscount::set_rate(const unsigned &tenor, const double &rate) {
    m_rate[tenor] = rate;
    m_rate_tenor[tenor] = rate * tenor / 365.0;
}

} // namespace minirisk
