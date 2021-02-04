#include <iostream>
#include <chrono>
#include "bignumber.h"

using namespace std;
using namespace chrono;

// https://leetcode.com/problems/divide-two-integers/discuss/338701/C%2B%2B-32-bit-binary-long-division-algorithm-O

BigInt::BigInt long_division(BigInt::BigInt dividend, BigInt::BigInt divisor)
{
    BigInt::BigInt result(0);
    while(dividend >= divisor)
    {
        int diff = (dividend.List().size() - divisor.List().size());
        auto temp = divisor << diff;

        if(temp > dividend)
        {
            temp >>= 1;
            --diff;
        }

        dividend -= temp;
        result += BigInt::BigInt(1)<<diff;
    }

    return result;
}
int main() {
    BigInt::BigInt a(543789534);
    BigInt::BigInt b(6543879);

    auto start = high_resolution_clock::now();
    auto res = long_division(a,b);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "long_division " << duration.count() << endl;

    return 0;
}

