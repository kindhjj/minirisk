#pragma once
#include "ICurve.h"

namespace minirisk
{

struct Market;

struct CurveFXSpot : ICurveFXSpot
{
    virtual string name() const { return m_name; }

    virtual Date today() const { return m_today; }

    CurveFXSpot(Market *mkt, const Date& today, const string& curve_name);

    CurveFXSpot(Market *mkt, const Date &today, const double rate);

    double spot() const;

private:
    Date m_today;
    string m_name;

    //map that stores the FX spot rate per foreign ccy
    double m_fx_spot;
};

} // namespace minirisk