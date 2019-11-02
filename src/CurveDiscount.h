#pragma once
#include "ICurve.h"

namespace minirisk {

struct Market;

struct CurveDiscount : ICurveDiscount
{
    virtual string name() const { return m_name; }

    CurveDiscount(Market *mkt, const Date& today, const string& curve_name);

    // compute the discount factor
    double df(const Date& t) const;

    virtual Date today() const { return m_today; }

private:
    Date   m_today;
    string m_name;
    double m_rate;
    // unsigned m_expr;
    // unsigned m_expr_date;
    // double m_df;
};

} // namespace minirisk
