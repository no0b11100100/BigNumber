//#include <iostream>
//#include "bignumber.h"
//#include <chrono>

//using namespace std;
//using namespace chrono;

//int main()
//{
//    BigInt::BigInt a(2634578);
//    auto start = high_resolution_clock::now();
//    BigInt::BigInt add = a + 126543;
//    auto stop = high_resolution_clock::now();
////    add.Print();
//    auto duration = duration_cast<microseconds>(stop - start);
//    cout << "add " << duration.count() << endl;

//    start = high_resolution_clock::now();
//    BigInt::BigInt mult = a * 126543;
//    stop = high_resolution_clock::now();
////    mult.Print();
//    duration = duration_cast<microseconds>(stop - start);
//    cout << "mult " << duration.count() << endl;

//    start = high_resolution_clock::now();
//    BigInt::BigInt sub = a - 126543;
//    stop = high_resolution_clock::now();
////    sub.Print();
//    duration = duration_cast<microseconds>(stop - start);
//    cout << "sub " << duration.count() << endl;

//    start = high_resolution_clock::now();
//    BigInt::BigInt div = a / 126543;
//    stop = high_resolution_clock::now();
////    div.Print();
//    duration = duration_cast<microseconds>(stop - start);
//    cout << "div " << duration.count() << endl;

//}


//#include <iostream>
//#include <chrono>

//using namespace std;
//using namespace chrono;

//int main() {
//    int a = 30;
//    int b = 18;

//    auto start = high_resolution_clock::now();
//    while (a != 0 && b != 0) // https://ru.wikipedia.org/wiki/%D0%91%D0%B8%D0%BD%D0%B0%D1%80%D0%BD%D1%8B%D0%B9_%D0%B0%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC_%D0%B2%D1%8B%D1%87%D0%B8%D1%81%D0%BB%D0%B5%D0%BD%D0%B8%D1%8F_%D0%9D%D0%9E%D0%94
//    {
//        if (a > b) a = a % b;
//        else b = b % a;
//    }

//    auto stop = high_resolution_clock::now();
//    auto duration = duration_cast<nanoseconds>(stop - start);
//    cout << "% " << duration.count() << endl;

//    a = 30;
//    b = 7;

//    start = high_resolution_clock::now();
//    int count = 0;
//    while (a != b)
//    {
//        ++count;
//        if (a > b) {
//            a = a - b;
//        }
//        else {
//            b = b - a;
//        }
//    }


//    stop = high_resolution_clock::now();
//    duration = duration_cast<nanoseconds>(stop - start);
//    cout << "- " << duration.count() << endl;

//    cout << count << endl;

//    return 0;
//}

#include <iostream>
#include <chrono>
#include <cassert>
#include "bignumber.h"

using namespace std;
using namespace chrono;

// https://www.geeksforgeeks.org/divide-two-integers-without-using-multiplication-division-mod-operator/
BigInt::BigInt divide_logn(BigInt::BigInt dividend, BigInt::BigInt divisor) {
    BigInt::BigInt quotient, temp;
    BigInt::BigInt MaxValue(1);
    int size = std::max(dividend.List().size(), divisor.List().size());
    for (int i = size; i >= 0; --i) {
        if (temp + (divisor << i) > dividend)
        {
            temp += (divisor << i);
            quotient |= (MaxValue << i);
        }
    }

    return quotient;
}

int main() {
    BigInt::BigInt a(100);
    BigInt::BigInt b(3);

    auto start = high_resolution_clock::now();
    auto res = divide_logn(a, b);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "divide_logn " << duration.count() << endl;

    res.Print();

    return 0;
}

