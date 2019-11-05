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

std::pair<double, string> PricerPayment::price(Market& mkt, const string& str) const
{
    double df = 0.0;
    bound_t lower_bound, higher_bound;
    lower_bound.first = 0;
    higher_bound.first = (Date::n_years + 1) * 366;
    bool tenor_ticker = false; //false if IR tenor shorter than the payment.
    unsigned dt = time_frac(mkt.today(), m_dt);
    MYASSERT((!(m_dt < mkt.today())), "Curve " << m_ir_curve << ", DF not available before anchor date " << mkt.today().to_string(true) << ", requested " << m_dt.to_string(true)); // this throws an exception if m_dt<today
    if (str == "compute pv")
    {
        std::shared_ptr<const MarketDataServer> m_mds = mkt.get_mds();
        std::vector<string> ccy_tenor = m_mds->match_ir(m_ir_curve.substr(m_ir_curve.length() - 3));
        for (auto tenor_i : ccy_tenor)
        {
            ptr_disc_curve_t disc = mkt.get_discount_curve(tenor_i);
            string m_name(disc->name());
            get_interval(mkt, lower_bound, higher_bound, tenor_ticker, m_name, dt);
        }
    }
    else if (str == "compute pv01")
    {
        vec_risk_factor_t ir_rf = mkt.get_risk_factors("IR\\..*" + m_ir_curve.substr(ir_curve_discount_prefix.length()));
        for (auto rf : ir_rf){
            string m_name(rf.first);
            get_interval(mkt, lower_bound, higher_bound, tenor_ticker, m_name, dt);
        }
    }
    MYASSERT(tenor_ticker, "Curve " << m_ir_curve << ", DF not available beyond last tenor date " << Date(lower_bound.first + mkt.today().get_serial()).to_string(true) << ", requested " << m_dt.to_string(true));
    double forward_rate = (higher_bound.second - lower_bound.second) / (higher_bound.first - lower_bound.first) * 365.0;
    df = std::exp(-lower_bound.second - forward_rate * (dt - lower_bound.first) / 365.0);

    // This PV is expressed in m_ccy. It must be converted in USD.
    if (!m_fx_ccy.empty())
        df *= mkt.get_fx_spot(m_fx_ccy);

    return std::make_pair(m_amt * df, string());
}

void PricerPayment::get_interval(const Market& mkt, bound_t& lower_bound, bound_t& higher_bound, bool& tenor_ticker, const string& m_name, const unsigned& dt) const
{
    std::pair<double, unsigned> tr = mkt.get_term_rate_and_tenor(m_name);
    unsigned tmp_v = tr.second;
    double tenor_rate = tr.first;
    if (lower_bound.first < tmp_v && tmp_v <= dt)
    {
        lower_bound.first = tmp_v;
        lower_bound.second = tenor_rate;
    }
    else if (higher_bound.first > tmp_v && dt < tmp_v)
    {
        higher_bound.first = tmp_v;
        higher_bound.second = tenor_rate;
        tenor_ticker = true;
    }
}

} // namespace minirisk


