#include <iomanip>
#include <algorithm>

#include "Date.h"

namespace minirisk {

struct DateInitializer : std::array<unsigned, Date::n_years>
{
    DateInitializer()
    {
        for (unsigned i = 0, s = 0, y = Date::first_year; i < size(); ++i, ++y) {
            (*this)[i] = s;
            s += 365 + (Date::is_leap_year(y) ? 1 : 0);
        }
    }
};

const std::array<unsigned, 12> Date::days_in_month = { {31,28,31,30,31,30,31,31,30,31,30,31} };
const std::array<unsigned, 12> Date::days_ytd{ {0,31,59,90,120,151,181,212,243,273,304,334} };
const std::array<unsigned, Date::n_years> Date::days_epoch(static_cast<const std::array<unsigned, Date::n_years>&>(DateInitializer()));

/* The function checks if a given year is a leap year.
    Leap year must be a multiple of 4, but it cannot be a multiple of 100 without also being a multiple of 400.
*/
bool Date::is_leap_year(unsigned year)
{
    return ((year % 4 != 0) ? false : (year % 100 != 0) ? true : (year % 400 != 0) ? false : true);
}

// The function pads a zero before the month or day if it has only one digit.
std::string Date::padding_dates(unsigned month_or_day)
{
    std::ostringstream os;
    os << std::setw(2) << std::setfill('0') << month_or_day;
    return os.str();
}

void Date::check_valid(unsigned y, unsigned m, unsigned d)
{
    MYASSERT(y >= first_year, "The year must be no earlier than year " << first_year << ", got " << y);
    MYASSERT(y < last_year, "The year must be smaller than year " << last_year << ", got " << y);
    MYASSERT(m >= 1 && m <= 12, "The month must be a integer between 1 and 12, got " << m);
    unsigned dmax = days_in_month[m - 1] + ((m == 2 && is_leap_year(y)) ? 1 : 0);
    MYASSERT(d >= 1 && d <= dmax, "The day must be a integer between 1 and " << dmax << ", got " << d);
}

void Date::check_valid(unsigned input_serial){
    MYASSERT(input_serial >= 0 && input_serial < days_epoch.back(), "Input date " << std::to_string(input_serial) << " out of bound.");
}

unsigned Date::day_of_year() const
{
    return days_ytd[m_m - 1] + ((m_m > 2 && m_is_leap) ? 1 : 0) + (m_d - 1);
}

// calculate m_y,m_m,m_d from serial number
void Date::calculate_ymd(){
    unsigned days_remain;
    auto find_year = std::adjacent_find(days_epoch.begin(), days_epoch.end(), [&](unsigned v1, unsigned v2) -> bool {return v1 <= m_serial && m_serial < v2; });
    m_y += static_cast<unsigned>(std::distance(days_epoch.begin(), find_year));
    days_remain = m_serial - *find_year + 1;
    unsigned month = 1;
    for (auto i = days_in_month.cbegin(); days_remain > *i; i++)
    {
        if (is_leap_year(m_y) && month == 2){
            if (days_remain > 29) { 
                days_remain -= 29;
                month++;
            }}
        else{
                days_remain -= *i;
                month ++ ;
            }
    }
    m_d = static_cast<unsigned char>(days_remain);
    m_m = static_cast<unsigned char>(month);
}

/*  The function calculates the distance between two Dates.
    d1 > d2 is allowed, which returns the negative of d2-d1.
*/
long operator-(const Date& d1, const Date& d2)
{
    unsigned s1 = d1.get_serial();
    unsigned s2 = d2.get_serial();
    return static_cast<long>(s1) - static_cast<long>(s2);
}

} // namespace minirisk

