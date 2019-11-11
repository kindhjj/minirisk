#include "Global.h"
#include "PortfolioUtils.h"
#include "TradePayment.h"

#include <numeric>


namespace minirisk {

void print_portfolio(const portfolio_t& portfolio)
{
    std::for_each(portfolio.begin(), portfolio.end(), [](auto& pt){ pt->print(std::cout); });
}

std::vector<ppricer_t> get_pricers(const portfolio_t& portfolio)
{
    std::vector<ppricer_t> pricers(portfolio.size());
    std::transform( portfolio.begin(), portfolio.end(), pricers.begin()
                  , [](auto &pt) -> ppricer_t { return pt->pricer(); } );
    return pricers;
}

portfolio_values_t compute_prices(const std::vector<ppricer_t>& pricers, Market& mkt)
{
    portfolio_values_t prices;
    prices.reserve(pricers.size());
    for (unsigned i = 0; i < pricers.size(); i++)
    {
        try{
            prices.emplace_back(pricers[i]->price(mkt));
        }
        catch (const std::exception &e){
            prices.emplace_back(std::make_pair(std::numeric_limits<double>::quiet_NaN(), e.what()));
        }
    }
        return prices;
}

portfolio_values_nan_t portfolio_total(const portfolio_values_t& values)
{
    portfolio_nan_vec nan_portf;
    nan_portf.reserve(values.size());
    double sum_m = 0.0;
    for (unsigned i = 0; i < values.size(); i++){
        if (std::isnan(values[i].first))
            nan_portf.emplace_back(std::make_pair(i, values[i].second));
        else
            sum_m += values[i].first;
        }

    return std::make_pair(sum_m, nan_portf);

}

//compute bucketed pv01
std::vector<std::pair<string, portfolio_values_t>> compute_pv01_bucketed(const std::vector<ppricer_t> &pricers, const Market &mkt)
{
    std::vector<std::pair<string, portfolio_values_t>> pv01_bucketed; // PV01 per trade

    const double bump_size = 0.01 / 100;

    // filter risk factors related to IR
    auto base = mkt.get_risk_factors(ir_rate_prefix + "[0-9]{1,2}[DWMY]\\.[A-Z]{3}");

    // Make a local copy of the Market object, because we will modify it applying bumps
    // Note that the actual market objects are shared, as they are referred to via pointers
    // Market tmpmkt(mkt);

    // compute prices for perturbated markets and aggregate results
    pv01_bucketed.reserve(base.size());
    Market tmpmkt(mkt);
    for (const auto &d : base)
    {
        std::vector<std::pair<double, string>> pv_up, pv_dn;
        std::vector<std::pair<string, double>> bumped(1, d);
        pv01_bucketed.emplace_back(std::make_pair(d.first, std::vector<std::pair<double, string>>(pricers.size())));

        // bump down and price
        bumped[0].second = d.second - bump_size;
        tmpmkt.set_risk_factors(bumped);
        pv_dn = compute_prices(pricers, tmpmkt);

        // bump up and price
        bumped[0].second = d.second + bump_size; // bump up
        tmpmkt.set_risk_factors(bumped);
        pv_up = compute_prices(pricers, tmpmkt);

        // restore original market state for next iteration
        // (more efficient than creating a new copy of the market at every iteration)
        bumped[0].second = d.second;
        tmpmkt.set_risk_factors(bumped);

        // compute estimator of the derivative via central finite differences
        double dr = 2.0 * bump_size;
        std::transform(pv_up.begin(), pv_up.end(), pv_dn.begin(), pv01_bucketed.back().second.begin(),
                       [dr](std::pair<double, string> hi, std::pair<double, string> lo) -> std::pair<double, string> {
                           if (!std::isnan(lo.first) && !std::isnan(hi.first))
                               return std::make_pair((hi.first - lo.first) / dr, string());
                           else
                               return lo;
                       });
    }

    return pv01_bucketed;
}

//compute parallel pv01
std::vector<std::pair<string, portfolio_values_t>> compute_pv01_parallel(const std::vector<ppricer_t>& pricers, const Market& mkt)
{
    std::vector<std::pair<string, portfolio_values_t>> pv01_parallel;
    std::vector<std::pair<double, string>> pv_up, pv_dn;
    const double bump_size = 0.01 / 100;
    Market tmpmkt(mkt);
    std::set<string> ccy_set;
    std::for_each(pricers.begin(), pricers.end(), [&ccy_set](ppricer_t pp) { ccy_set.insert(pp->get_ir_curve()); });
    pv01_parallel.reserve(ccy_set.size());
    for (auto cs : ccy_set){
        pv01_parallel.emplace_back(std::make_pair(ir_rate_prefix + cs.substr(cs.length() - 3), std::vector<std::pair<double, string>>(pricers.size())));
        Market::vec_risk_factor_t risk_factor_ccy = mkt.get_risk_factors(ir_rate_prefix + ".*" + cs.substr(cs.length() - 3));
        std::vector<std::pair<string, double>> bumped;
        bumped.reserve(risk_factor_ccy.size());

        //bump down and price
        for (auto rf_ccy : risk_factor_ccy)
            bumped.emplace_back(std::make_pair(rf_ccy.first, rf_ccy.second - bump_size));
        tmpmkt.set_risk_factors(bumped);
        pv_dn = compute_prices(pricers, tmpmkt);

        //bump up and price
        for (auto &each_bump : bumped)
            each_bump.second += 2.0 * bump_size;
        tmpmkt.set_risk_factors(bumped);
        pv_up = compute_prices(pricers, tmpmkt);

        // restore original market state for next iteration
        for (auto &each_bump : bumped)
            each_bump.second -= bump_size;
        tmpmkt.set_risk_factors(bumped);

        double dr = 2.0 * bump_size;
        std::transform(pv_up.begin(), pv_up.end(), pv_dn.begin(), pv01_parallel.back().second.begin(), 
                [dr](std::pair<double, string> hi, std::pair<double, string> lo) -> std::pair<double, string> 
        {
            if (!std::isnan(lo.first) && !std::isnan(hi.first)) return std::make_pair((hi.first - lo.first) / dr, string());
            else return lo;
        });
    }
    return pv01_parallel;
}

std::vector<std::pair<string, portfolio_values_t>> compute_fx_delta(const std::vector<ppricer_t> &pricers, const Market &mkt)
{
    std::vector<std::pair<string, portfolio_values_t>> fx_delta;

    const double bump_size = 0.1 / 100;

    // filter fx factors
    auto base = mkt.get_risk_factors(fx_spot_prefix + "[A-Z]{3}");

    fx_delta.reserve(base.size());
    Market tmpmkt(mkt);

    for (const auto &d : base)
    {
        std::vector<std::pair<double, string>> pv_up, pv_dn;
        std::vector<std::pair<string, double>> bumped(1, d);
        fx_delta.emplace_back(std::make_pair(d.first, std::vector<std::pair<double, string>>(pricers.size())));

        // bump down and price
        bumped[0].second = d.second - bump_size;
        tmpmkt.set_fx_risk_factors(bumped);
        pv_dn = compute_prices(pricers, tmpmkt);

        // bump up and price
        bumped[0].second = d.second + bump_size;
        tmpmkt.set_fx_risk_factors(bumped);
        pv_up = compute_prices(pricers, tmpmkt);

        bumped[0].second = d.second;
        tmpmkt.set_fx_risk_factors(bumped);

        // compute estimator of the derivative via central finite differences
        double dr = 2.0 * bump_size;
        std::transform(pv_up.begin(), pv_up.end(), pv_dn.begin(), fx_delta.back().second.begin(),
                       [dr](std::pair<double, string> hi, std::pair<double, string> lo) -> std::pair<double, string> {
                           if (!std::isnan(lo.first) && !std::isnan(hi.first))
                               return std::make_pair((hi.first - lo.first) / dr, string());
                           else
                               return lo;
                       });
    }

    return fx_delta;
}

ptrade_t load_trade(my_ifstream& is)
{
    string name;
    ptrade_t p;

    // read trade identifier
    guid_t id;
    is >> id;

    if (id == TradePayment::m_id)
        p.reset(new TradePayment);
    else
        THROW("Unknown trade type:" << id);

    p->load(is);

    return p;
}

void save_portfolio(const string& filename, const std::vector<ptrade_t>& portfolio)
{
    // test saving to file
    my_ofstream of(filename);
    for( const auto& pt : portfolio) {
        pt->save(of);
        of.endl();
    }
    of.close();
}

std::vector<ptrade_t> load_portfolio(const string& filename)
{
    std::vector<ptrade_t> portfolio;

    // test reloading the portfolio
    my_ifstream is(filename);
    while (is.read_line())
        portfolio.push_back(load_trade(is));

    return portfolio;
}

void print_price_vector(const string& name, const portfolio_values_t& values)
{
    std::pair<double, std::vector<std::pair<size_t, string>>> m_total_cec = portfolio_total(values);
    std::vector<std::pair<size_t, string>> nan_portf = m_total_cec.second;
    std::cout
        << "========================\n"
        << name << ":\n"
        << "========================\n"
        << "Total:  " << m_total_cec.first
        << "\nErrors: " << nan_portf.size() << "\n"
        << "\n========================\n";

    for (size_t i = 0, n = values.size(); i < n; ++i)
        if (std::isnan(values[i].first))
            std::cout << std::setw(5) <<  i << ": " << values[i].second << "\n";
        else std::cout << std::setw(5) << i << ": " << values[i].first << "\n";

    std::cout << "========================\n\n";
}

} // namespace minirisk
