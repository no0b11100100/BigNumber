#pragma once
#include <list>
#include <vector>
#include <deque>
#include <string>
#include <iostream>
#include <exception>
// https://handwiki.org/wiki/Computational_complexity_of_mathematical_operations
// http://numbers.computation.free.fr/Constants/Programs/programs.html
namespace BigNumber
{
class BigNumber
{
    std::list<int> m_number;
    std::list<int> m_fraction;

    template<typename T, class = typename std::enable_if_t<std::is_same_v<T, int> || std::is_same_v<T, std::size_t> > >
    void fromInt(T number)
    {
        while(number > 0)
        {
            m_number.push_front(number%10);
            number /= 10;
        }
    }

    void fromString(std::string number)
    {
        validateNumber(number);
        bool isFraction = false;
        for(auto digit : number)
        {
            if(digit == '.') isFraction = true;

            if(!isFraction) m_number.push_front(digit-'0');
            else m_fraction.push_front(digit-'0');
        }
    }

    void fromDouble(double number)
    {
        fromString(std::to_string(number));
    }

    void fromVector(const std::vector<int>& number)
    {
        validateNumber(number);
        for(auto& digit : number)
        {
            m_number.push_front(digit);
        }
    }

    void fromList(const std::list<int>& number)
    {
        validateNumber(number);
        for(auto& digit : number)
        {
            m_number.push_front(digit);
        }
    }

    void fromDeque(const std::deque<int>& number)
    {
        validateNumber(number);
        for(auto& digit : number)
        {
            m_number.push_front(digit);
        }
    }

    // For container
    template<class T>
    void validateNumber(T number)
    {
        bool first = false;
        for(auto digit : number)
        {
            if(first == false)
            {
                first = true;
                if(digit > 9 || digit < -9) throw std::runtime_error("Invalid number");
            }
            if(digit > 9 || digit < 0) throw std::runtime_error("Invalid number");
        }
    }

    void validateNumber(std::string number)
    {
        char fisrtDigit = number[0];

        if(fisrtDigit < '0' || fisrtDigit > '9' || fisrtDigit != '-') throw std::runtime_error("Invalid number");
        if(fisrtDigit == '-' && number[1] == '.') throw std::runtime_error("Invalid number");

        for(std::size_t i = 1, dotCounter = 0; i < number.length(); ++i)
        {
            if(number[i] < '0' || number[i] > '9' || number[i] != '.') throw std::runtime_error("Invalid number");
            if(number[i] == '.')
            {
                ++dotCounter;
                if(dotCounter > 1) throw std::runtime_error("Invalid number");
            }
        }
    }

public:

    BigNumber(int number);
    BigNumber(std::size_t number);
    BigNumber(double number);
    BigNumber(float number);
    BigNumber(std::string number);

    BigNumber& operator+(BigNumber);
    BigNumber& operator+(int);
    BigNumber& operator+(std::size_t);
    BigNumber& operator+(double);
    BigNumber& operator+(float);
    BigNumber& operator+(std::string);

    //TODO: remake to fit my needs
    std::string longDivision(std::string number, int divisor) // one more https://www.geeksforgeeks.org/program-quotient-remainder-big-number/
    {
        std::string ans;
        std::size_t idx = 0;
        int temp = number[idx] - '0';
        while (temp < divisor)
            // binary search?
            temp = temp * 10 + (number[++idx] - '0');

        // Repeatedly divide divisor with temp. After
        // every division, update temp to include one
        // more digit.
        while (number.size() > idx) {
            // Store result in answer i.e. temp / divisor
            ans += (temp / divisor) + '0';

            // Take next digit of number
            temp = (temp % divisor) * 10 + number[++idx] - '0';
        }

        // If divisor is greater than number
        if (ans.length() == 0)
            return "0";

        // else return ans
        return ans;
    }

    BigNumber& operator-(BigNumber);
    BigNumber& operator*(BigNumber); // Алгоритм Фюрера или Карацубы http://hardfire.ru/fft_mul https://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B http://acm.mipt.ru/twiki/bin/view/Algorithms/FastFourierTransformCPP
    BigNumber &operator/(BigNumber); // https://www.ideone.com/Q3omVw https://habr.com/ru/post/172285/ https://www.youtube.com/watch?v=dmXqCqz9Y_0 https://medium.com/@dip_kush04/operations-on-big-integers-and-finding-the-modulo-multiplicative-inverse-6b6e934fbe60
                                    // https://www.more-magic.net/posts/numeric-tower-part-3.html
    BigNumber operator%(int);

//    template<typename T>
//    BigNumber& pow(BigNumber, T degree);

//    template<typename T, typename Mod>
//    BigNumber& pow_mod(BigNumber, T degree, Mod modulo);

    bool operator<(BigNumber);
    bool operator<=(BigNumber);
    bool operator>(BigNumber);
    bool operator>=(BigNumber);
    bool operator==(BigNumber);
    bool operator!=(BigNumber);
    std::ostream& operator<<(BigNumber);
    std::istream& operator>>(int);

    std::string toString() const;
    std::list<int> toList() const;
    std::vector<int> toVector() const;
    std::deque<int> toDeque() const;
    bool isMultiples2() const;
    bool isMultiples3() const;
    bool isMultiples4() const; // 2 послдение цифры делятся на 4
    bool isMultiples5() const;
    bool isMultiples7() const;
    bool isMultiples8() const; // 3 послдение цифры делятся на 8
    bool isMultiples11() const;
    bool isMultiples13() const;
};

} // BigNumber

// https://habr.com/ru/post/262705/
// https://infourok.ru/dlinnaya-arifmetika-na-c-opisanie-modeli-realizaciya-zadachi-1959820.html
//#include <cstdlib>   // for rand()
//#include <iostream>  // for cout
//#include <math.h>    // for pow()
//#include <stdio.h>   // for printf()
//#define TEST
//#define D_MAX 729
//#define D     729
//using namespace std;
//class Calc
//{
//    int A[D];
//    int B[D];
//#ifdef TEST
//    int cnt_mul;
//    clock_t t1, t2;
//    double tt;
//#endif
//    public:
//        Calc();
//        void calcToomCook();
//    private:
//        void multiplyNormal(int *, int *, int, int *);
//        void multiplyToomCook3(int *, int *, int , int *);
//        void doCarry(int *, int);
//        void display(int *, int *, int *);
//};
//Calc::Calc()
//{
//     int i;
//    for (i = 0; i < D; i++)
//    {
//        A[i] = rand() % 10;
//        B[i] = rand() % 10;
//    }
//}
//void Calc::calcToomCook()
//{
//    int a[D_MAX];
//    int b[D_MAX];
//    int z[D_MAX * 2];
//    int i;
//#ifdef TEST
//    t1 = clock();
//    for (int l = 0; l < 1000; l++) {
//        cnt_mul = 0;
//#endif
//        for (i = 0; i < D; i++)
//    {
//        a[i] = A[i];
//        b[i] = B[i];
//    }
//        for (i = D; i < D_MAX; i++)
//    {
//        a[i] = 0;
//        b[i] = 0;
//    }
//    multiplyToomCook3(a, b, D_MAX, z);
//    doCarry(z, D_MAX * 2);
//#ifdef TEST
//    }
//    t2 = clock();
//    tt = (double)(t2 - t1) / CLOCKS_PER_SEC;
//#endif
//display(a, b, z);
//}
///*
// * Multiplication Standard
// */
//void Calc::multiplyNormal(int *a, int *b, int tLen, int *z)
//{
//    int i, j;
//   for(i = 0; i < tLen * 2; i++) z[i] = 0;
//   for (j = 0; j < tLen; j++) {
//        for (i = 0; i < tLen; i++) {
//            z[j + i] += a[i] * b[j];
//#ifdef TEST
//            cnt_mul++;
//#endif
//        }
//    }
//}
//void Calc::multiplyToomCook3(int *a, int *b, int tLen, int *z)
//{
//    int *a0 = &a[0];                // Multiplicand / right side array pointer
//    int *a1 = &a[tLen / 3];         // Multiplicand / central array pointer
//    int *a2 = &a[tLen * 2/ 3];      // Multiplicand / left side array pointer
//    int *b0 = &b[0];                // Multiplier / right side array pointer
//    int *b1 = &b[tLen / 3];         // Multiplier / central array pointer
//    int *b2 = &b[tLen * 2/ 3];      // Multiplier / left side array pointer
//    int *c0 = &z[(tLen / 3) *  0];  // c0
//    int *c2 = &z[(tLen / 3) *  2];  // c2
//    int *c4 = &z[(tLen / 3) *  4];  // c4
//    int c1      [(tLen / 3) * 2];   // c1
//    int c3      [(tLen / 3) * 2];   // c3
//    int a_m2    [tLen / 3];         // a( -2)
//    int a_m1    [tLen / 3];         // a( -1)
//    int a_0     [tLen / 3];         // a(  0)
//    int a_1     [tLen / 3];         // a(  1)
//    int a_inf   [tLen / 3];         // a(inf)
//    int b_m2    [tLen / 3];         // b( -2)
//    int b_m1    [tLen / 3];         // b( -1)
//    int b_0     [tLen / 3];         // b(  0)
//    int b_1     [tLen / 3];         // b(  1)
//    int b_inf   [tLen / 3];         // b(inf)
//    int c_m2    [(tLen / 3) * 2];   // c( -2)
//    int c_m1    [(tLen / 3) * 2];   // c( -1)
//    int c_0     [(tLen / 3) * 2];   // c(  0)
//    int c_1     [(tLen / 3) * 2];   // c(  1)
//    int c_inf   [(tLen / 3) * 2];   // c(inf)
//    int i;
//    if (tLen <= 9)
//    {
//        multiplyNormal(a, b, tLen, z);
//        return;
//    }
//    // ==== a(-2) = 4 * a2 - 2 * a1 + a0, b(1) = 4 * b2 - 2 * b1 + b0
//    for(i = 0; i < tLen / 3; i++)
//    {
//        a_m2[i] = (a2[i] << 2) - (a1[i] << 1) + a0[i];
//        b_m2[i] = (b2[i] << 2) - (b1[i] << 1) + b0[i];
//    }
//    // ==== a(-1) = a2 - a1 + a0, b(1) = b2 - b1 + b0
//    for(i = 0; i < tLen / 3; i++)
//    {
//        a_m1[i] = a2[i] - a1[i] + a0[i];
//        b_m1[i] = b2[i] - b1[i] + b0[i];
//    }
//    // ==== a(0) = a0, b(0) = b0
//    for(i = 0; i < tLen / 3; i++) {
//        a_0[i] = a0[i];
//        b_0[i] = b0[i];
//    }
//    // ==== a(1) = a2 + a1 + a0, b(1) = b2 + b1 + b0
//    for(i = 0; i < tLen / 3; i++) {
//        a_1[i] = a2[i] + a1[i] + a0[i];
//        b_1[i] = b2[i] + b1[i] + b0[i];
//    }
//    // ==== a(inf) = a2, b(inf) = b2
//    for(i = 0; i < tLen / 3; i++) {
//        a_inf[i] = a2[i];
//        b_inf[i] = b2[i];
//    }
//    // ==== c(-2) = a(-2) * b(-2)
//    multiplyToomCook3(a_m2,  b_m2,  tLen / 3, c_m2 );
//    // ==== c(-1) = a(-1) * b(-1)
//    multiplyToomCook3(a_m1,  b_m1,  tLen / 3, c_m1 );
//    // ==== c(0) = a(0) * b(0)
//    multiplyToomCook3(a_0,   b_0,   tLen / 3, c_0  );
//    // ==== c(1) = a(1) * b(1)
//    multiplyToomCook3(a_1,   b_1,   tLen / 3, c_1  );
//    // ==== c(inf) = a(inf) * b(inf)
//    multiplyToomCook3(a_inf, b_inf, tLen / 3, c_inf);
//    // ==== c4 = 6 * c(inf) / 6
//    for(i = 0; i < (tLen / 3) * 2; i++)
//        c4[i] = c_inf[i];
//        // ==== c3 = -c(-2) + 3 * c(-1) - 3 * c(0) + c(1) + 12 * c(inf) / 6
//    for(i = 0; i < (tLen / 3) * 2; i++)
//    {
//        c3[i]  = -c_m2[i];
//        c3[i] += (c_m1[i] << 1) + c_m1[i];
//        c3[i] -= (c_0[i] << 1) + c_0[i];
//        c3[i] += c_1[i];
//        c3[i] += (c_inf[i] << 3) + (c_inf[i] << 2);
//        c3[i] /= 6;
//    }
//    // ==== c2 = 3 * c(-1) - 6 * c(0) + 3 * c(1) - 6 * c(inf) / 6
//    for(i = 0; i < (tLen / 3) * 2; i++) {
//        c2[i]  = (c_m1[i] << 1) + c_m1[i];
//        c2[i] -= (c_0[i] << 2) + (c_0[i] << 1);
//        c2[i] += (c_1[i] << 1) + c_1[i];
//        c2[i] -= (c_inf[i] << 2) + (c_inf[i] << 1);
//        c2[i] /= 6;
//    }
//    // ==== c1 = c(-2) - 6 * c(-1) + 3 * c(0) + 2 * c(1) - 12 * c(inf) / 6
//    for(i = 0; i < (tLen / 3) * 2; i++) {
//        c1[i]  = c_m2[i];
//        c1[i] -= (c_m1[i] << 2) + (c_m1[i] << 1);
//        c1[i] += (c_0[i] << 1) + c_0[i];
//        c1[i] += (c_1[i] << 1);
//        c1[i] -= (c_inf[i] << 3) + (c_inf[i] << 2);
//        c1[i] /= 6;
//    }
//    // ==== c0 = 6 * c(0) / 6
//    for(i = 0; i < (tLen / 3) * 2; i++)
//        c0[i] = c_0[i];
//    // ==== z = c4 * x^4 + c3 * x^3 + c2 * x^2 + c1 * x + c0
//    for(i = 0; i < (tLen / 3) * 2; i++) z[i + tLen / 3] += c1[i];
//    for(i = 0; i < (tLen / 3) * 2; i++) z[i + (tLen / 3) * 3] += c3[i];
//}
//void Calc::doCarry(int *a, int tLen) {
//    int cr;
//    int i;
//    cr = 0;
//    for(i = 0; i < tLen; i++) {
//        a[i] += cr;
//        if(a[i] < 0) {
//            cr = -(-(a[i] + 1) / 10 + 1);
//        } else {
//            cr = a[i] / 10;
//        }
//        a[i] -= cr * 10;
//    }
//    // Overflow
//    if (cr != 0) printf("[ OVERFLOW!! ] %d\n", cr);
//}
///*
// * Result output
// */
//void Calc::display(int *a, int *b, int *z)
//{
//    int i;
//    int aLen = D_MAX, bLen = D_MAX, zLen = D_MAX * 2;
//    while (a[aLen - 1] == 0) if (a[aLen - 1] == 0) aLen--;
//    while (b[bLen - 1] == 0) if (b[bLen - 1] == 0) bLen--;
//    while (z[zLen - 1] == 0) if (z[zLen - 1] == 0) zLen--;
//    // a
//    printf("a =\n");
//    for (i = aLen - 1; i >= 0; i--) {
//        printf("%d", a[i]);
//        if ((aLen - i) % 10 == 0) printf(" ");
//        if ((aLen - i) % 50 == 0) printf("\n");
//    }
//    printf("\n");
//    // b
//    printf("b =\n");
//    for (i = bLen - 1; i >= 0; i--) {
//        printf("%d", b[i]);
//        if ((bLen - i) % 10 == 0) printf(" ");
//        if ((bLen - i) % 50 == 0) printf("\n");
//    }
//    printf("\n");
//    // z
//    printf("z =\n");
//    for (i = zLen - 1; i >= 0; i--) {
//        printf("%d", z[i]);
//        if ((zLen - i) % 10 == 0) printf(" ");
//        if ((zLen - i) % 50 == 0) printf("\n");
//    }
//    printf("\n\n");
//#ifdef TEST
//    printf("Counts of multiply / 1 loop = %d\n", cnt_mul);     // Multiplication count
//    printf("Total time of all loops     = %f seconds\n", tt);  // processing time
//#endif
//}
//int main()
//{
//    try
//    {
//        Calc objCalc;
//        objCalc.calcToomCook();
//    }
//    catch (...) {
//        cout << "Exception occurred!" << endl;
//        return -1;
//    }
//    return 0;
//}

//#include <iostream>
//using namespace std;
//int noOfDigit(long x) {
//   int n = 0;
//   while (x > 0) {
//      x /= 10;
//      n++;
//   }
//   return n;
//}
//void schonhageStrassenMultiplication(long a, long b, int n, int m) { // https://martin-thoma.com/strassen-algorithm-in-python-java-cpp/
//   int linearConvolution[n + m - 1];
//   for (int i = 0; i < (n + m - 1); i++)
//      linearConvolution[i] = 0;
//      long p = a;
//   for (int i = 0; i < m; i++) {
//      a = p;
//      for (int j = 0; j < n; j++) {
//         linearConvolution[i + j] += (b % 10) * (a % 10);
//         a /= 10;
//      }
//      b /= 10;
//   }
//   cout << "The Linear Convolution is: ( ";
//   for (int i = (n + m - 2); i >= 0; i--) {
//      cout << linearConvolution[i] << " ";
//   }
//   cout << ")";
//   long product = 0;
//   int nextCarry = 0, base = 1;
//   for (int i = 0; i < n + m - 1; i++) {
//      linearConvolution[i] += nextCarry;
//      product = product + (base * (linearConvolution[i] % 10));
//      nextCarry = linearConvolution[i] / 10;
//      base *= 10;
//   }
//   cout << "\nThe Product of the numbers is: " << product;
//}
//int main(int argc, char **argv) {
//   cout << "Enter the numbers:";
//   long a, b;
//   cin >> a >> b;
//   int n = noOfDigit(a);
//   int m = noOfDigit(b);
//   schonhageStrassenMultiplication(a, b, n, m);

//    return 0;

//}

//#include <iostream>
//#include <cassert> // для assert()
//// A helper function for finding the position of the most significant set bit.
//// Please plug in your intrinsic of choice here
//static unsigned int find_first_bit(uint32_t value) {
//#   ifdef _MSC_VER
//    unsigned long index;
//    (void) _BitScanReverse(&index, value);
//    return index + 1;
//#   else
//    unsigned int count = 0;
//    for(count = 0; value; ++count)
//        value >>= 1;
//    return count;
//#   endif
//}

//// Multi-word division in 32-bit big-endian segments, returning the most significant
//// word of the quotient.
//uint32_t divide(const uint32_t *num, const uint32_t *den, size_t len) {
//    // Skip past leading zero denominator digits. The quotient must fit in a single
//    // 32-bit word and so only the preceeding numerator digit needs be examined
//    uint32_t norm_den;
//    uint64_t norm_num = 0;
//    size_t top = 0;
//    while(norm_den = den[top], !norm_den)
//        norm_num = num[top++];
//    // Please pad the input to insure at least three denominator words counting from
//    // the first non-zero digit
//    assert(len >= top + 3);
//    // Divide the first two digits of the numerator by the leading digit of the
//    // denominator as an initial quotient digit guess, yielding an upper bound
//    // for the quotient at most two steps above the true value.
//    // Simultaneously normalize the denominator with the MSB in the 31st bit.
//    unsigned int norm = find_first_bit(norm_den);
//    norm_num = norm_num << (64 - norm);
//    norm_num |= ((uint64_t) num[top + 0] << 32 | num[top + 1]) >> norm;
//    norm_den = ((uint64_t) norm_den << 32 | den[top + 1]) >> norm;
//    // We are using a straight 64/64 division where 64/32=32 would suffice. The latter
//    // is available on e.g. x86-32 but difficult to generate short of assembly code.
//    uint32_t quot = (uint32_t) (norm_num / norm_den);
//    // Substitute norm_num - quot * norm_den if your optimizer is too thick-headed to
//    // efficiently extract the remainder
//    uint32_t rem = norm_num % norm_den;
//    // Test the next word of the input, reducing the upper bound to within one step
//    // of the true quotient. See Knuth for proofs of this reduction and the bounds
//    // of the first guess
//    norm_num = ((uint64_t) num[top + 1] << 32 | num[top + 2]) >> norm;
//    norm_num = (uint64_t) rem << 32 | (uint32_t) norm_num;
//    norm_den = ((uint64_t) den[top + 1] << 32 | den[top + 2]) >> norm;
//    if((uint64_t) quot * norm_den > norm_num) {
//        --quot;
//        // There is no "add-back" step try to avoid and so there is little point
//        // in looping to refine the guess further since the bound is sufficiently
//        // tight already
//    }
//    // Compare quotient guess multiplied by the denominator to the numerator
//    // across whole numbers to account for the final quotient step.
//    // There is no need to bother with normalization here. Furthermore we can
//    // compare from the most to the least significant and cut off early when the
//    // intermediate result becomes large, yielding a constant average running
//    // time for random input
//    uint64_t accum = 0;
//    do {
//        uint64_t prod = (uint64_t) quot * *den++;
//        accum = accum << 32 | *num++;
//        // A negative partial result can never recover, so pick the lower
//        // quotient. A separate test is required to avoid 65-bit arithmetic
//        if(accum < prod)
//            return --quot;
//        accum -= prod;
//        // Similarly a partial result which spills into the upper 32-bits can't
//        // recover either, so go with the upper quotient
//        if((uint64_t) accum >= 0x100000000)
//            return quot;
//    } while(--len);
//    return quot;
//}

//int main()
//{
//	const uint32_t a[] = {1244, 3324, 4321, 6543};
//	const uint32_t b[] = {44, 3324, 4321, 4235};
//	std::cout << divide(a, b, 4) << std::endl;
//	return 0;
//}

// pow
//bool big_integer::odd() const {
//        if (this->_digits.size() == 0) return false;
//        return this->_digits[0] & 1;
//}

//bool big_integer::even() const {
//        return !this->odd();
//}

//const big_integer big_integer::pow(big_integer n) const {
//        big_integer a(*this), result(1);
//        while (n != 0) {
//                if (n.odd()) result *= a;
//                a *= a;
//                n /= 2;
//        }

//        return result;
//}
