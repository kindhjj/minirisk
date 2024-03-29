#pragma once

#include "Macros.h"
#include <string>
#include <array>

namespace minirisk {

struct Date
{
public:
    static const unsigned first_year = 1900;
    static const unsigned last_year = 2200;
    static const unsigned n_years = last_year - first_year + 1;

private:

    // number of days elapsed from beginning of the year
    unsigned day_of_year() const;

    friend long operator-(const Date& d1, const Date& d2);

    static const std::array<unsigned, 12> days_in_month;  // num of days in month M in a normal year
    static const std::array<unsigned, 12> days_ytd;      // num of days since 1-jan to 1-M in a normal year
    static const std::array<unsigned, n_years> days_epoch;   // num of days since 1-jan-1900 to 1-jan-yyyy (until 2200)

public:
    // Default constructor
    Date() : m_y(1970), m_m(1), m_d(1), m_is_leap(false), m_serial(this->to_serial()) {}

    // Constructor where the input value is checked.
    Date(unsigned year, unsigned month, unsigned day)
    {
        init(year, month, day);
    }

    // Constructor where the input value is serial.
    Date(unsigned input_serial){
        init(input_serial);
    }

    void init(unsigned year, unsigned month, unsigned day)
    {
        check_valid(year, month, day);
        m_y = (unsigned short) year;
        m_m = (unsigned char) month;
        m_d = (unsigned char) day;
        m_is_leap = is_leap_year(year);
        m_serial = (unsigned)this->to_serial();
    }

    void init(unsigned input_serial){
        check_valid(input_serial);
        m_y = 1900;
        m_serial = input_serial;
        calculate_ymd();
    }

    static void check_valid(unsigned y, unsigned m, unsigned d);
    static void check_valid(unsigned date_serial);

    bool operator<(const Date& d) const
    {
        // return (m_y < d.m_y) || (m_y == d.m_y && (m_m < d.m_m || (m_m == d.m_m && m_d < d.m_d)));
        return (this->m_serial) < d.m_serial;
    }

    bool operator==(const Date& d) const
    {
        // return (m_y == d.m_y) && (m_m == d.m_m) && (m_d == d.m_d);
        return (this->m_serial) == d.m_serial;
    }

    bool operator>(const Date& d) const
    {
        // return d < (*this);
        return (this->m_serial) > d.m_serial;
    }

    // get m_serial
    unsigned get_serial() const {
        return m_serial;
    }

    static bool is_leap_year(unsigned yr);

    static std::string padding_dates(unsigned month_or_day);

    void calculate_ymd();

    // True: In DD-MM-YYYY format; False: YYYYMMDD
    std::string to_string(bool pretty = true) const
    {
        return pretty
                ? std::to_string(m_d) + "-" + std::to_string(m_m) + "-" + std::to_string(m_y)
                //: std::to_string(m_y) + padding_dates(m_m) + padding_dates(m_d);
                : std::to_string(m_serial);
    }

private:
    unsigned m_y;
    unsigned char m_m;
    unsigned char m_d;
    bool m_is_leap;
    unsigned m_serial;  //the corresponding serial date number

    // number of days since 1-Jan-1900
    unsigned to_serial() const
    {
        return days_epoch[m_y - 1900] + day_of_year();
    }
};

long operator-(const Date& d1, const Date& d2);

inline unsigned time_frac(const Date& d1, const Date& d2)
{
    return static_cast<unsigned>(d2 - d1);
}

} // namespace minirisk
