#pragma once

#include "Global.h"
#include "IObject.h"
#include "ICurve.h"
#include "MarketDataServer.h"
#include <vector>
#include <regex>

namespace minirisk {

struct Market : IObject
{
private:
    // NOTE: this function is not thread safe
    template <typename I, typename T>
    std::shared_ptr<const I> get_curve(const string& name);

    template <typename I, typename T>
    std::shared_ptr<const I> get_fxsp(const string &name);

    template <typename I, typename T>
    std::shared_ptr<const I> get_fxforward(const string &ccy1, const string &ccy2);

    double from_mds(const string& objtype, const string& name);

public:

    typedef std::pair<string, double> risk_factor_t;
    typedef std::vector<std::pair<string, double>> vec_risk_factor_t;

    Market(){}
    
    Market(const std::shared_ptr<const MarketDataServer>& mds, const Date& today)
        : m_today(today)
        , m_mds(mds)
    {
    }

    virtual Date today() const { return m_today; }

    // get an object of type ICurveDisocunt
    const ptr_disc_curve_t get_discount_curve(const string& name);

    // yield rate for currency name
    const std::map<unsigned, double> get_yield(const string &ccyname);

    // fx exchange rate to convert 1 unit of ccy1 into USD
    const double get_fx_spot(const string& ccy);

    const ptr_fxsp_t get_fx_ptr(const string& ccy);

    const ptr_fxfw_curve_t get_fxforward_ptr(const string &ccy1, const string &ccy2);

    // after the market has been disconnected, it is no more possible to fetch
    // new data points from the market data server
    void disconnect()
    {
        m_mds.reset();
    }

    const std::shared_ptr<const MarketDataServer> get_mds() const { return m_mds; }

    // returns risk factors matching a regular expression
    vec_risk_factor_t get_risk_factors(const std::string& expr) const;

    // clear all market curves execpt for the data points
    void clear()
    {
        std::for_each(m_curves.begin(), m_curves.end(), [](auto& p) { p.second.reset(); });
    }

    // destroy all existing objects and modify a selected number of data points
    void set_risk_factors(const vec_risk_factor_t& risk_factors);

    void set_fx_risk_factors(const vec_risk_factor_t &risk_factors);

    const unsigned transferdate(const string &tenor_sub);

    const bool find_ccy_rate(const string& ccy) const;

    //reset curve according to risk factors
    template <typename I, typename T>
    void reset_curve(const std::pair<string, double>& rf);

    void reset_spot();

private:
    Date m_today;
    std::shared_ptr<const MarketDataServer> m_mds;

    // market curves
    std::map<string, ptr_curve_t> m_curves;

    // fx spot rate
    std::map<string, ptr_fxsp_t> m_fxsp;

    // raw risk factors
    std::map<string, double> m_risk_factors;

    // FX forward
    std::map<string, ptr_fxfw_curve_t> m_fxforward;
};

} // namespace minirisk

