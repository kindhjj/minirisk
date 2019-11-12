
#include "CurveFXForward.h"
#include "Global.h"
#include "Market.h"

namespace minirisk
{

// name in the format "CCY1.CCY2""
CurveFXForward::CurveFXForward(
    Market *mkt, const Date &today, const std::string &name)
    : m_today(today), m_name(fx_forward_prefix + name)
{
    string ccy1 = name.substr(0, 3);
    string ccy2 = name.substr(4);
    m_ir1 = mkt->get_discount_curve(ccy1);
    m_ir2 = mkt->get_discount_curve(ccy2);
    m_fxspot_ccy1 = mkt->get_fx_ptr(fx_spot_name(ccy1, ccy2));
    m_fxspot_ccy2 = mkt->get_fx_ptr(fx_spot_name(ccy2, ccy1));
}

double CurveFXForward::fwd(const Date &dt) const
{
    return m_fxspot_ccy1->spot() / m_fxspot_ccy2->spot() * m_ir1->df(dt, dt.get_serial()) / m_ir2->df(dt, dt.get_serial());
}

} // namespace minirisk