#pragma once

#include <memory>
#include <string>
#include <map>

#include "IObject.h"
#include "Date.h"

using std::string;

namespace minirisk {

// forward declarations
struct Market;
struct MarketDataServer;

struct ICurve : IObject
{
    virtual string name() const = 0;
    virtual Date today() const = 0;
};

// forward declaration
struct ICurveDiscount;

typedef std::shared_ptr<const ICurve> ptr_curve_t;
typedef std::shared_ptr<const ICurveDiscount> ptr_disc_curve_t;

struct ICurveDiscount : ICurve
{
    virtual double df(const Date& m_dt, const unsigned& dt) const = 0;
    virtual void set_rate(const unsigned &tenor, const double &rate) = 0;
};

struct ICurveFXForward : ICurve
{
    // compute the FX forward price of currency ccy1 deniminated in ccy2 for delivery at time t
    virtual double fwd(const Date& t) const = 0;
};

struct ICurveFXSpot : ICurve
{
    // return the FX spot price of currency ccy1 deniminated in ccy2 for delivery at time t
    // ignore the fact that the FX spot is a t+2 forward price itself, and assume it is the istantaneous exchange rate
    virtual double spot() const = 0;
};

    
} // namespace minirisk

