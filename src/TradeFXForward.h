#pragma once

#include "Trade.h"

namespace minirisk
{

struct TradeFXForward : Trade<TradeFXForward>
{
    friend struct Trade<TradeFXForward>;

    static const guid_t m_id;
    static const std::string m_name;

    TradeFXForward() {}

    void init(const std::string &ccy1, const std::string &ccy2,
              const double quantity, const double strike, const Date &fixing_date,
              const Date &settle_date)
    {
        Trade::init(quantity);
        m_ccy1 = ccy1;
        m_ccy2 = ccy2;
        m_strike = strike;
        m_fixing_date = fixing_date;
        m_settle_date = settle_date;
        m_serial_id = get_counting();
    }

    virtual ppricer_t pricer() const;

    const string &ccy1() const
    {
        return m_ccy1;
    }

    const string &ccy2() const
    {
        return m_ccy2;
    }

    const Date &fixing_date() const
    {
        return m_fixing_date;
    }

    const Date &settle_date() const
    {
        return m_settle_date;
    }

    const double strike() const
    {
        return m_strike;
    }

    const unsigned serial() const
    {
        return m_serial_id;
    }

private:
    void save_details(my_ofstream &os) const
    {
        std::stringstream ofs;
        ofs << std::hex << m_q_hex.h;
        string tmp_str;
        ofs >> tmp_str;
        os << m_ccy1 << m_ccy2 << tmp_str << m_fixing_date << m_settle_date;
    }

    void load_details(my_ifstream &is)
    {
        string strike_str;
        is >> m_ccy1 >> m_ccy2 >> strike_str>> m_fixing_date >> m_settle_date;
        std::istringstream tmp_str(strike_str);
        tmp_str >> std::hex >> m_q_hex.h;
        m_strike = m_q_hex.d;
        
    }

    void print_details(std::ostream &os) const
    {
        os << format_label("Strike level") << m_strike << std::endl;
        os << format_label("Base Currency") << m_ccy1 << std::endl;
        os << format_label("Quote Currency") << m_ccy2 << std::endl;
        os << format_label("Fixing Date") << m_fixing_date << std::endl;
        os << format_label("Settlement Date") << m_settle_date << std::endl;
    }

private:
    string m_ccy1;
    string m_ccy2;
    double m_strike;
    Date m_fixing_date;
    Date m_settle_date;
    union{double d; uint64_t h;} m_q_hex;
    unsigned m_serial_id;
};

} // namespace minirisk
