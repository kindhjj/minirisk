#include "PricerPayment.h"
#include "TradePayment.h"
#include "CurveDiscount.h"
#include <cmath>

namespace minirisk {

PricerPayment::PricerPayment(const TradePayment& trd)
    : m_amt(trd.quantity())
    , m_dt(trd.delivery_date())
    , m_ir_curve(ir_curve_discount_name(trd.ccy()))
    , m_fx_ccy(trd.ccy() == "USD" ? "" : fx_spot_name(trd.ccy(),"USD"))
{
}

std::pair<double, string> PricerPayment::price(Market& mkt) const
{
    unsigned dt = time_frac(mkt.today(), m_dt);
    // this throws an exception if m_dt<today
    MYASSERT((!(m_dt < mkt.today())), "Curve " << m_ir_curve << ", DF not available before anchor date " << mkt.today().to_string(true) << ", requested " << m_dt.to_string(true)); 
    string ccy_name = m_ir_curve.substr(m_ir_curve.length() - 3);
    ptr_disc_curve_t disc = mkt.get_discount_curve(ccy_name);
    double df_n = disc->df(m_dt, dt);

    // This PV is expressed in m_ccy. It must be converted in USD.
    if (!m_fx_ccy.empty())
        df_n *= mkt.get_fx_spot(m_fx_ccy);

    return std::make_pair(m_amt * df_n, string());
}

} // namespace minirisk


