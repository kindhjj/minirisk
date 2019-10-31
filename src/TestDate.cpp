#include "Date.h"
#include <iostream>
#include <time.h>
#include <cstdlib>
#include <string>
#include "Macros.h"
#include <vector>
#include <array>

bool is_valid_date(unsigned& y, unsigned& m, unsigned& d, unsigned MAX_VALID_YR=2200, unsigned MIN_VALID_YR=1900) 
{ 
    // If year, month and day are not in given range 
    if (y > MAX_VALID_YR ||  y < MIN_VALID_YR) return false; 
    if (m < 1 || m > 12) return false; 
    if (d < 1 || d > 31) return false; 
    // Handle February month with leap year 
    if (m == 2) { 
        if (minirisk::Date::is_leap_year(y)) return (d <= 29); 
        else return (d <= 28); 
    } 
    // Months of April, June, Sept and Nov must have number of days less than or equal to 30. 
    if (m == 4 || m == 6 || m == 9 || m == 11) return (d <= 30); 
    return true; 
} 

void generate_dates(std::vector<std::array<unsigned,3>>& dates){
    unsigned d = 1;
    unsigned m = 1;
    unsigned y = 1900;
    while (y < 2200){
        std::array<unsigned,3> date = {y, m, d};
        dates.push_back(date);
        d++;
        if (!is_valid_date(y,m,d)) {
            d = 1;
            m++;
        };
        if (m > 12) {
            m = 1;
            y++;
        }; 
    }
}
void test1()
{
    //time range the same as is defined in Date struct
    const unsigned MAX_VALID_YR = 2200; 
    const unsigned MIN_VALID_YR = 1900; 
    const unsigned MAX_VALID_MH = 13;
    const unsigned MAX_VALID_DY = 35;
    srand((unsigned)time(NULL));    //initiate seed
    unsigned n = 0; //invalid date generated
    unsigned e_n = 0;   //invalid date recognized

    do{
        unsigned y = rand() % (MAX_VALID_YR - MIN_VALID_YR) + MIN_VALID_YR; //generate random date
        unsigned m = rand() % MAX_VALID_MH + 1;
        unsigned d = rand() % MAX_VALID_DY + 1;
        if (is_valid_date(y, m, d, MAX_VALID_YR, MIN_VALID_YR)==false){
            n++;
            try{
                minirisk::Date date(y, m, d);
            }catch (const std::exception& msg){
                e_n++;
            }
        }
    } while (n < 1000);
    if (n != e_n){
        std::string msg("Test 1 failed: Generated 1000 invalid dates, but only ");
        msg.append(std::to_string(e_n));
        msg.append(" errors were recognized");
        MYASSERT(0, msg);
    }
}

void test2()
{
    std::vector<std::array<unsigned,3>> dates;
    generate_dates(dates);
    for (auto i1 = dates.cbegin(); i1 != dates.cend(); i1++){
        std::array<unsigned, 3> i2_n = *i1;
        auto i2 = i2_n.cbegin();
        std::cout << *i2 << "-" << *(i2 + 1) << "-" << *(i2 + 2) << "\n";
    }
}

void test3()
{
    std::vector<std::array<unsigned,3>> dates;
    generate_dates(dates);
    for (auto i1 = dates.cbegin(); i1 != dates.cend(); i1++){
        std::array<unsigned, 3> i2_n = *i1;
        auto i2 = i2_n.cbegin();
        minirisk::Date m_date(*i2, *(i2 + 1), *(i2 + 2));
        unsigned date_serial_pre = m_date.serial();
    }

    int main()
    {
        try
        {
            test1();
            test2();
            test3();
        }
        catch (const std::exception &msg_all)
        {
            std::cout << msg_all.what();
            return 1;
    }
    std::cout << "SUCCESS.\n";
    return 0;
}

