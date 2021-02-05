#include <iostream>
#include <chrono>
#include "bignumber.h"

using namespace std;
using namespace chrono;

int main() {
    BigInt::BigInt a(543789534);
    BigInt::BigInt b(6543879);

    auto start = high_resolution_clock::now();

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "long_division " << duration.count() << endl;


    return 0;
}

