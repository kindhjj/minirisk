#include "Market.h"
#include "CurveDiscount.h"
#include "CurveFXSpot.h"
#include "CurveFXForward.h"

#include <vector>

namespace minirisk {

template <typename I, typename T>
std::shared_ptr<const I> Market::get_curve(const string& name)   //copy discount curve from mds to mkt
{
    ptr_curve_t &curve_ptr = m_curves[name];
    if (!curve_ptr.get())
        curve_ptr.reset(new T(this, m_today, name.substr(name.length() - 3)));
    std::shared_ptr<const I> res = std::dynamic_pointer_cast<const I>(curve_ptr);
    MYASSERT(res, "Cannot cast object with name " << name << " to type " << typeid(I).name());
    return res;
}

template <typename I, typename T>
std::shared_ptr<const I> Market::get_fxsp(const string& name)   //read fx spot from mds to mkt
{
    ptr_fxsp_t &fx_ptr = m_fxsp[name];
    if (!fx_ptr.get())
        fx_ptr.reset(new T(this, m_today, name));
    std::shared_ptr<const I> res = std::dynamic_pointer_cast<const I>(fx_ptr);
    MYASSERT(res, "Cannot cast object with name " << name << " to type " << typeid(I).name());
    return res;
}

template <typename I, typename T>
std::shared_ptr<const I> Market::get_fxforward(const string &ccy1, const string &ccy2)
{
    string fx_pair = fx_forward_name(ccy1, ccy2);
    ptr_fxfw_curve_t &fxf_ptr = m_fxforward[fx_pair];
    if (!fxf_ptr.get())
        fxf_ptr.reset(new T(this, m_today, ccy1 + "." + ccy2));
    std::shared_ptr<const I> res = std::dynamic_pointer_cast<const I>(fxf_ptr);
    MYASSERT(res, "Cannot cast object with name " << fx_pair << " to type " << typeid(I).name());
    return res;
}

const ptr_disc_curve_t Market::get_discount_curve(const string& name)
{
    return get_curve<ICurveDiscount, CurveDiscount>(ir_rate_prefix + name);
}

double Market::from_mds(const string& objtype, const string& name)
{
    std::pair<std::map<string, double>::iterator, bool> ins;
    if (name == fx_spot_prefix + "USD")
        ins = m_risk_factors.emplace(fx_spot_prefix + base_ccy, std::numeric_limits<double>::quiet_NaN());
    else
        ins = m_risk_factors.emplace(name, std::numeric_limits<double>::quiet_NaN());
    if (ins.second) { // just inserted, need to be populated
        MYASSERT(m_mds, "Cannot fetch " << objtype << " " << name << " because the market data server has been disconnnected");
        if (objtype=="yield curve")
        {
            ins.first->second = m_mds->get(name);
            return ins.first->second;
        }
        else if (objtype=="fx spot")
        {
            if (base_ccy != "USD")
            {
                if (name == fx_spot_prefix + "USD")
                {
                    ins.first->second = m_mds->get(fx_spot_prefix + base_ccy);
                    return 1.0 / ins.first->second;
                }
                else
                {
                    ins.first->second = m_mds->get(name);
                    return ins.first->second / m_mds->get(fx_spot_prefix + base_ccy);
                }
            }
            else
                return ins.first->second = m_mds->get(name);
        }
    }
    else
    {
        if (objtype == "fx spot")
        {
            if (base_ccy != "USD")
                return (name.substr(name.length()-3)=="USD" ? 1.0 : m_risk_factors.find(name)->second) / (m_risk_factors.find(fx_spot_prefix + base_ccy)->second);
            else
                return m_risk_factors.find(name)->second;
        }
    }
    MYASSERT(0, "Unkown error getting rates.")
}

const std::map<unsigned, double> Market::get_yield(const string& ccyname)
{
    std::map<unsigned, double> yield_curve;
    string name(ir_rate_prefix + ccyname);
    std::vector<string> ir_tenor_name = m_mds->get_ir_vector(ccyname);
    for (auto ir_tn : ir_tenor_name)
        yield_curve[transferdate(ir_tn.substr(0, ir_tn.length() - 4))] = from_mds("yield curve", ir_rate_prefix + ir_tn);
    yield_curve.insert(std::make_pair(0, 0.0));
    return yield_curve;
};

const double Market::get_fx_spot(const string& name)
{
    return from_mds("fx spot", name);
}

const ptr_fxsp_t Market::get_fx_ptr(const string& ccy)
{
    return get_fxsp<ICurveFXSpot, CurveFXSpot>(mds_spot_name(ccy));
}

const ptr_fxfw_curve_t Market::get_fxforward_ptr(const string &ccy1, const string &ccy2)
{
    return get_fxforward<ICurveFXForward, CurveFXForward>(ccy1, ccy2);
}

void Market::set_risk_factors(const vec_risk_factor_t& risk_factors)
{
    //clear();
    for (const auto& d : risk_factors) {
        auto i = m_risk_factors.find(d.first);
        MYASSERT((i != m_risk_factors.end()), "Risk factor not found " << d.first);
        i->second = d.second;
        reset_curve<ICurveDiscount, ICurve>(d);
    }
}

void Market::set_fx_risk_factors(const vec_risk_factor_t &risk_factors)
{
    for (const auto& d : risk_factors) {
        auto i = m_risk_factors.find(d.first);
        MYASSERT((i != m_risk_factors.end()), "Risk factor not found " << d.first);
        i->second = d.second;
    }
    reset_spot();
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

const bool Market::find_ccy_rate(const string& ccy) const{
    std::regex find_ccy("IR\\..*" + ccy);
    std::smatch full_matched;
    bool if_full_match=false;
    for (auto m_rf : m_risk_factors){
        if_full_match = std::regex_match(m_rf.first, full_matched, find_ccy);
        if (if_full_match)
            return true;
    }
    return false;
}

template <typename I, typename T>
void Market::reset_curve(const std::pair<string, double>& rf){
    string ccy = rf.first.substr(rf.first.length() - 3);
    unsigned tenor = transferdate(rf.first.substr(3, rf.first.length() - 7));
    auto found_curve = m_curves.find(ir_rate_prefix + ccy);
    MYASSERT(found_curve != m_curves.end(), "No such risk factors found at curve: " <<  ir_rate_prefix + ccy);
    std::shared_ptr<const T> found_c = found_curve->second;
    std::shared_ptr<const I> found_curve_ptr = std::dynamic_pointer_cast<const I>(found_c);
    const ICurveDiscount *ic_c = found_curve_ptr.get();
    ICurveDiscount *ic = const_cast<ICurveDiscount *>(ic_c);
    ic->set_rate(tenor, rf.second);
}

void Market::reset_spot()
{
    std::for_each(m_fxsp.begin(), m_fxsp.end(), [](auto& p) { p.second.reset();});
    std::for_each(m_fxforward.begin(), m_fxforward.end(), [](auto& p) { p.second.reset();});
}

} // namespace minirisk
