#include "CurveDiscount.h"
#include "Market.h"
#include "Streamer.h"

#include <cmath>


namespace minirisk {

CurveDiscount::CurveDiscount(Market *mkt, const Date& today, const string& curve_name)
    : m_today(today)
    , m_name(ir_rate_prefix + curve_name)    
    , m_rate(mkt->get_yield(curve_name))
    , m_tenor(mkt->transferdate(curve_name.substr(0,curve_name.length() - 4)))
{
    m_rate_tenor = m_rate * m_tenor / 365.0;
}

double  CurveDiscount::df() const
{
    return m_rate;
}

double CurveDiscount::get_rate_tenor() const { return m_rate_tenor; }

unsigned CurveDiscount::get_tenor() const { return m_tenor; }

void CurveDiscount::set_rate(const double& rate) {
    m_rate = rate;
    m_rate_tenor = m_rate * m_tenor / 365.0;
}

} // namespace minirisk
