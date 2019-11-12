#pragma once

#include "IPricer.h"
#include "TradePayment.h"

namespace minirisk {

struct PricerPayment : IPricer
{
    typedef std::pair<string, double> risk_factor_t;
    typedef std::vector<std::pair<string, double>> vec_risk_factor_t;
    typedef std::pair<unsigned, double> bound_t;

    PricerPayment(const TradePayment& trd);

    virtual std::pair<double, string> price(Market &mkt, ptr_fds_t &fds) const;

    string get_ir_curve() const { return m_ir_curve; }

private:
    double m_amt;
    Date   m_dt;
    string m_ir_curve;
    string m_fx_ccy;
    unsigned m_serial_id;
};

} // namespace minirisk

