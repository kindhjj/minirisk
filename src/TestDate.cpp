#include "Date.h"
#include <iostream>
#include <time.h>
#include <cstdlib>
#include <string>
#include "Macros.h"

bool is_valid_date(unsigned& y, unsigned& m, unsigned& d, unsigned MAX_VALID_YR, unsigned MIN_VALID_YR) 
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
        std::string msg("Generated 1000 invalid dates, but only ");
        msg.append(std::to_string(e_n));
        msg.append(" errors were recognized");
        try{ MYASSERT(0, msg); } catch (const std::exception& msg_all){
            std::cout << msg_all.what();
        }
    };
}

void test2()
{

}

void test3()
{
}

int main()
{
    test1();
    test2();
    test3();
    std::cout << "SUCCESS.\n";
    return 0;
}

