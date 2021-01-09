#include <iostream>
#include "bignumber.h"

using namespace std;

int main()
{
    BigNumber::BigNumber a(12342);
    BigNumber::BigNumber b(876);

    auto res = a+b;

    cout << res.toString() << endl;

    res = res + 10;
    cout << res.toString() << endl;

    res = res - 30;
    cout << res.toString() << endl;

    res = res - 45;
    cout << res.toString() << endl;
}
