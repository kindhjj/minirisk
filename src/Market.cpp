#include "Market.h"
#include "CurveDiscount.h"

#include <vector>

namespace minirisk {

template <typename I, typename T>
std::shared_ptr<const I> Market::get_curve(const string& name)   //copy discount curve from mds to mkt
{
    ptr_curve_t &curve_ptr = m_curves[ir_rate_prefix + name];
    if (!curve_ptr.get())
        curve_ptr.reset(new T(this, m_today, name));
    std::shared_ptr<const I> res = std::dynamic_pointer_cast<const I>(curve_ptr);
    MYASSERT(res, "Cannot cast object with name " << name << " to type " << typeid(I).name());
    return res;
}

const ptr_disc_curve_t Market::get_discount_curve(const string& name)
{
    return get_curve<ICurveDiscount, CurveDiscount>(name);
}

double Market::from_mds(const string& objtype, const string& name)
{
    auto ins = m_risk_factors.emplace(name, std::numeric_limits<double>::quiet_NaN());
    if (ins.second) { // just inserted, need to be populated
        MYASSERT(m_mds, "Cannot fetch " << objtype << " " << name << " because the market data server has been disconnnected");
        if (objtype=="yield curve")
            ins.first->second = m_mds->get(name);
        else if (objtype=="fx spot")
            ins.first->second = m_mds->get(name);
    }
    return ins.first->second;
}

const double Market::get_yield(const string& ccyname)
{
    string name(ir_rate_prefix + ccyname);
    return from_mds("yield curve", name);
};

const double Market::get_fx_spot(const string& name)
{
    return from_mds("fx spot", mds_spot_name(name));
}

void Market::set_risk_factors(const vec_risk_factor_t& risk_factors)
{
    //clear();
    for (const auto& d : risk_factors) {
        auto i = m_risk_factors.find(d.first);
        MYASSERT((i != m_risk_factors.end()), "Risk factor not found " << d.first);
        i->second = d.second;
        //std::pair<string, double> rf_pair = std::make_pair(d.first, d.second);
        reset_curve<ICurveDiscount, ICurve>(d);
    }
}

Market::vec_risk_factor_t Market::get_risk_factors(const std::string& expr) const
{
    vec_risk_factor_t result;
    std::regex r(expr);
    for (const auto& d : m_risk_factors)
        if (std::regex_match(d.first, r))
            result.push_back(d);
    return result;
}

//transform a "10M" like term to a number
const unsigned Market::transferdate(const string& tenor_sub){
    unsigned multiplier;
    switch (tenor_sub.c_str()[tenor_sub.length() - 1])
    {
    case 'D':
        multiplier = 1;
        break;
    case 'W':
        multiplier = 7;
        break;
    case 'M':
        multiplier = 30;
        break;
    case 'Y':
        multiplier = 365;
        break;
    default:
        MYASSERT(0, "Bad tenor type in market data: " << tenor_sub);
    }
    return static_cast<unsigned>(std::stoi(tenor_sub.substr(0, tenor_sub.length() - 1))) * multiplier;
}

//extract tenor info form a risk factor token
const std::pair<double, unsigned> Market::get_term_rate_and_tenor(const string& ccy) const{
    string f_name;
    if (ccy.substr(0, ir_curve_discount_prefix.length()) == ir_curve_discount_prefix)
        f_name = ir_rate_prefix + ccy.substr(ir_curve_discount_prefix.length());
    else if (ccy.substr(0, ir_rate_prefix.length()) == ir_rate_prefix)
        f_name = ccy;
    else
        f_name = ir_rate_prefix + ccy;
    auto d = m_curves.find(f_name);
    MYASSERT((d != m_curves.end()), "Term rate not found " << ccy);
    ptr_curve_t found_curve = d->second;
    ptr_disc_curve_t found_curve_ptr = std::dynamic_pointer_cast<const ICurveDiscount>(found_curve);
    return std::make_pair(found_curve_ptr->get_rate_tenor(), found_curve_ptr->get_tenor());
}

template <typename I, typename T>
void Market::reset_curve(const std::pair<string, double>& rf){
    auto found_curve = m_curves.find(rf.first);
    MYASSERT(found_curve != m_curves.end(), "No such risk factors found at curve: " << rf.first);
    std::shared_ptr<const T> found_c = found_curve->second;
    std::shared_ptr<const I> found_curve_ptr = std::dynamic_pointer_cast<const I>(found_c);
    const ICurveDiscount *ic_c = found_curve_ptr.get();
    ICurveDiscount *ic = const_cast<ICurveDiscount *>(ic_c);
    //sss->get_rateset_rate(iter.second);
    ic->set_rate(rf.second);
}
} // namespace minirisk
