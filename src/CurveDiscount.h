#pragma once
#include "ICurve.h"

namespace minirisk {

struct Market;

struct CurveDiscount : ICurveDiscount
{
    virtual string name() const { return m_name; }

    CurveDiscount(Market *mkt, const Date& today, const string& curve_name);

    // compute the discount factor
    double df() const;

    virtual Date today() const { return m_today; }

    double get_rate_tenor() const;

    unsigned get_tenor() const;

    void set_rate(const double &rate);

private:
    Date   m_today;
    string m_name;
    double m_rate;
    //store tenor in days
    unsigned m_tenor;
    //cache rate*tenor/365 terms
    double m_rate_tenor;
};

} // namespace minirisk
