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

//To generate a complete list of dates from 1900-1-1 to 2199-12-31
void generate_dates(std::vector<std::array<unsigned,3>>& dates){
    unsigned d = 1;
    unsigned m = 1;
    unsigned y = 1900;
    while (y < 2200){
        std::array<unsigned,3> date = {y, m, d};
        dates.emplace_back(date);
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
        unsigned y = (unsigned int)rand() % (MAX_VALID_YR - MIN_VALID_YR) + MIN_VALID_YR; //generate random date
        unsigned m = (unsigned int)rand() % MAX_VALID_MH + 1;
        unsigned d = (unsigned int)rand() % MAX_VALID_DY + 1;
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
        std::string msg("Test 1 failed: Generated 1000 invalid dates, but only " + std::to_string(e_n) + " errors were recognized");
        MYASSERT(0, msg);
    }
}

void test2(std::vector<std::array<unsigned,3>>& dates)
{
    unsigned date_counter = 0;
    for (auto i1 = dates.cbegin(); i1 != dates.cend(); i1++)
    {
        std::array<unsigned, 3> i2_n = *i1;
        auto i2 = i2_n.cbegin();
        minirisk::Date m_date(date_counter++);
        std::string str_date;
        str_date = std::to_string(*(i2 + 2)) + "-" + std::to_string(*(i2 + 1)) + "-" + std::to_string(*(i2));
        if (str_date != m_date.to_string())
        {
            //to check whether the output is consistent
            std::string msg("Test 2 failed:" + str_date+" not consistent.");
            MYASSERT(0, msg);
        }
    }
}

void test3(std::vector<std::array<unsigned,3>>& dates)
{
    minirisk::Date first_date((dates.at(0))[0], (dates.at(0))[1], (dates.at(0))[2]);
    //generate the first serial date
    unsigned date_serial_lead = first_date.get_serial();
    for (auto i1 = dates.cbegin() + 1; i1 != dates.cend(); i1++)
    {
        minirisk::Date m_date((*i1)[0], (*i1)[1], (*i1)[2]);
        unsigned date_serial_lag = m_date.get_serial();
        if (date_serial_lag-date_serial_lead!=1){
            std::string msg("Test 3 failed: " + m_date.to_string() + " not continuous with previous date.");
            MYASSERT(0, msg);
        }
        date_serial_lead = date_serial_lag;
    }
}

int main()
{
    try
    {
        test1();
        std::vector<std::array<unsigned,3>> dates;
        generate_dates(dates);
        test2(dates);
        test3(dates);
    }
    catch (const std::exception &msg_all)
    {
        std::cout << msg_all.what();
        return 1;
    };
    std::cout << "SUCCESS.\n";
    return 0;
}

