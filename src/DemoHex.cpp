#include <iostream>
#include <iomanip>
#include <cstdint>
#include <string>
#include <sstream>

using namespace std;

//void out_char_as_hex(int c)
//{
//    cout << hex << setw(2) << setfill('0') << c;
//}

int main()
{
    union { double d; uint64_t u; } tmp;
    istringstream st("4034000000000000");
    //double x = 10.0;
    st >> hex >> tmp.u;
    //tmp.d = x;
    //cout << hex << tmp.u << endl;
    cout << tmp.d;
    return 0;
}
