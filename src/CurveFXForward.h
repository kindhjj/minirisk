#pragma once

#include <string>

#include "ICurve.h"

namespace minirisk
{

struct Market;

struct CurveFXForward : ICurveFXForward
{
public:
    virtual std::string name() const { return m_name; }

    virtual Date today() const { return m_today; }

    CurveFXForward(Market *mkt, const Date &today, const std::string &name);

    virtual double fwd(const Date &t) const;

private:
    Date m_today;
    std::string m_name;
    ptr_disc_curve_t m_ir1;
    ptr_disc_curve_t m_ir2;
    ptr_fxsp_t m_fxspot_ccy1;
    ptr_fxsp_t m_fxspot_ccy2;
    std::map<unsigned, double> m_forward_rate;
}; // struct CurveFXForward

} // namespace minirisk