#pragma once

#include "IPricer.h"
#include "TradeFXForward.h"

namespace minirisk
{

struct PricerFXForward : IPricer
{
    PricerFXForward(const TradeFXForward &trd);

    //pricer function for fx forward struct
    virtual std::pair<double, string> price(Market &mkt, ptr_fds_t &fds) const;

    string get_ir_curve() const { return string(); };

    string swap_queing_ccy(string& name) const;

    std::pair<string,string> decompose_ccy(string& name) const;

private:
    double m_amt;
    double m_strike;
    string m_ccy1;
    string m_ccy2;
    Date m_fixing_date;
    Date m_settle_date;
    string m_fx_ccy;
    unsigned m_serial_id;
};

} // namespace minirisk