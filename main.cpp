#include <iostream>
#include "bignumber.h"
#include <map>

using namespace std;

int main()
{
    BigNumber::BigNumber a(100);
    BigNumber::BigNumber b(19);

//    auto res = a+b;

//    cout << res.toString() << endl;

//    res = res + 10;
//    cout << res.toString() << endl;

//    res = res - 30;
//    cout << res.toString() << endl;

//    res = res - 45;
//    cout << res.toString() << endl;

//    a = a - 10000;
//    cout << a.toString() << endl;

    auto r = a%19;
    cout << r.toString() << endl;

}
