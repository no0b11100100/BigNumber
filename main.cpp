#include <iostream>
#include <chrono>
#include "bignumber.h"

using namespace std;
using namespace chrono;

int main() {
    BigInt::BigInt a(12);
    BigInt::BigInt b(4);

    auto start = high_resolution_clock::now();
//    auto res = a*b.List();
    auto res = a-b;
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "\nmult " << duration.count() << endl << endl;
    cout << res.Decimal() << endl;

//    start = high_resolution_clock::now();
//    res = a*b;
//    stop = high_resolution_clock::now();
//    duration = duration_cast<microseconds>(stop - start);
//    cout << "\ntaby " << duration.count() << endl << endl;
//    cout << res.Decimal() << endl;

    return 0;
}

