#include <iostream>
#include "bignumber.h"

using namespace std;

int main()
{
    BigInt::BigInt a(26);
    BigInt::BigInt add = a + 12;
    add.Print();

    BigInt::BigInt mult = a * 12;
    mult.Print();
}
