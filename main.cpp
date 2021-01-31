#include <iostream>
#include "bignumber.h"
#include <chrono>

using namespace std;
using namespace chrono;

int main()
{
    BigInt::BigInt a(2634578);
    auto start = high_resolution_clock::now();
    BigInt::BigInt add = a + 126543;
    auto stop = high_resolution_clock::now();
//    add.Print();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "add " << duration.count() << endl;

    start = high_resolution_clock::now();
    BigInt::BigInt mult = a * 126543;
    stop = high_resolution_clock::now();
//    mult.Print();
    duration = duration_cast<microseconds>(stop - start);
    cout << "mult " << duration.count() << endl;

    start = high_resolution_clock::now();
    BigInt::BigInt sub = a - 126543;
    stop = high_resolution_clock::now();
//    sub.Print();
    duration = duration_cast<microseconds>(stop - start);
    cout << "sub " << duration.count() << endl;

    start = high_resolution_clock::now();
    BigInt::BigInt div = a / 126543;
    stop = high_resolution_clock::now();
//    div.Print();
    duration = duration_cast<microseconds>(stop - start);
    cout << "div " << duration.count() << endl;

}
