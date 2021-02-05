#include <iostream>
#include <chrono>
#include "bignumber.h"

using namespace std;
using namespace chrono;

BigInt::BigInt mult(BigInt::BigInt a, BigInt::BigInt b)
{
    BigInt::BigInt result;
    while(true)
    {
        if(b.is2Pow())
        {
            bool isOne = b.List().size() == 1 && *(b.List().begin()) == 1;
            return result + (isOne ?  a : (a << b.List().size()-1));
        }
        else
        {
            auto temp = b.toLow2Pow();
            result += a << temp.List().size()-1;
            b -= temp;
        }
    }
}

int main() {
    BigInt::BigInt a(20);
    BigInt::BigInt b(10);

    auto start = high_resolution_clock::now();
    auto res = mult(a,b);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "mult " << duration.count() << endl;
    cout << res.Decimal() << endl;

    start = high_resolution_clock::now();
    res = a*b;
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << "taby " << duration.count() << endl;
    cout << res.Decimal() << endl;

    return 0;
}

