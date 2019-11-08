#pragma once
#include "ICurve.h"

namespace minirisk {

struct Market;

struct CurveDiscount : ICurveDiscount
{
    virtual string name() const { return m_name; }

    CurveDiscount(Market *mkt, const Date& today, const string& curve_name);

    // compute the discount factor
    double df(const Date& m_dt, const unsigned& dt) const;

    virtual Date today() const { return m_today; }

    void set_rate(const unsigned &tenor, const double &rate);

private:
    Date   m_today;
    string m_name;
    //store tenor and rate in map
    std::map<unsigned, double> m_rate;
    //cache rate*tenor/365 terms
    std::map<unsigned, double> m_rate_tenor;
};

} // namespace minirisk
