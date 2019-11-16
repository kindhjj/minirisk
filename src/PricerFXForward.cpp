#include "PricerFXForward.h"

#include <cmath>

#include "Global.h"
#include "Macros.h"

namespace minirisk
{

PricerFXForward::PricerFXForward(
    const TradeFXForward &trd)
    : m_amt(trd.quantity()),
      m_strike(trd.strike()),
      m_ccy1(trd.ccy1()),
      m_ccy2(trd.ccy2()),
      m_fixing_date(trd.fixing_date()),
      m_settle_date(trd.settle_date()),
      m_fx_ccy(fx_spot_name(trd.ccy1(), trd.ccy2())),
      m_serial_id(get_counting())
{
}

std::pair<double, string> PricerFXForward::price(Market &mkt, ptr_fds_t &fds) const
{
    MYASSERT(!(mkt.today() > m_settle_date), "Curve " << ir_curve_discount_name(m_ccy2) << ", DF not available before anchor date " << mkt.today().to_string(true) << ", requested " << m_settle_date.to_string(true));
    Date m_sdt = time_frac(mkt.today(), m_settle_date);
    ptr_disc_curve_t disc = mkt.get_discount_curve(m_ccy2);
    double df_n = 0.0;
    // Historical Fixing
    // T1 < T0
    if (m_fixing_date < mkt.today())
    {
        double fixing_rate = fds->get(m_fx_ccy, m_fixing_date);
        df_n = disc->df(m_sdt, m_sdt.get_serial()) * (fixing_rate - m_strike);
        if (m_ccy2 != base_ccy)
            //get ccy2 fx spot rate
            df_n *= mkt.get_fx_ptr(fx_spot_name(m_ccy2, m_ccy1))->spot();
    }
    // T1 = T0 Historical Fixing, if not available use spot rate
    else if (m_fixing_date == mkt.today())
    {
        std::pair<double, bool> fixing_rate_pair = fds->lookup(m_fx_ccy, m_fixing_date);
        double fixing_rate, ccy1_spot, ccy2_spot;
        fixing_rate_pair.second ? fixing_rate = fixing_rate_pair.first : fixing_rate = 1.0;
        ccy1_spot = mkt.get_fx_ptr(m_fx_ccy)->spot();                     //get ccy1 fx spot rate
        ccy2_spot = mkt.get_fx_ptr(fx_spot_name(m_ccy2, m_ccy1))->spot(); //get ccy2 fx spot rate
        if (fixing_rate_pair.second)
            df_n = disc->df(m_sdt, m_sdt.get_serial()) * (fixing_rate - m_strike) * ccy2_spot;
        else
            df_n = disc->df(m_sdt, m_sdt.get_serial()) * (ccy1_spot / ccy2_spot - m_strike) * ccy2_spot;
    }
    // T1 > T0 Resolve price from forward curve
    else
    {
        ptr_fxfw_curve_t forward_ptr = mkt.get_fxforward_ptr(m_ccy1, m_ccy2);
        Date m_fdt = time_frac(mkt.today(), m_fixing_date);
        df_n = disc->df(m_sdt, m_sdt.get_serial()) * (forward_ptr->fwd(m_fdt) - m_strike);
        if (m_ccy2 != base_ccy)
            df_n *= mkt.get_fx_ptr(fx_spot_name(m_ccy2, m_ccy1))->spot(); //get ccy2 fx spot rate
    }

    return std::make_pair(m_amt * df_n, string());
}

} // namespace minirisk