#include "bignumber.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <complex>

BigNumber::BigNumber::BigNumber(int number)
{
    fromInt(number);
}

BigNumber::BigNumber::BigNumber(std::size_t number)
{
    fromInt(number);
}

BigNumber::BigNumber::BigNumber(float number)
{
    fromDouble(number);
}

BigNumber::BigNumber::BigNumber(std::string number)
{
    fromString(number);
}

BigNumber::BigNumber::BigNumber(std::list<int> number)
{
    fromList(number);
}

BigNumber::BigNumber::BigNumber(double number)
{
    fromDouble(number);
}

BigNumber::BigNumber &BigNumber::BigNumber::operator-(BigNumber number)
{
    auto num = number.toList();

    //TODO: make normal names
    auto main_start = rbegin(num);
    auto main_end = rend(num);
    auto second_start = rbegin(m_number);
    auto second_end = rend(m_number);

    bool isBigger = false;
    if (*this < number)
    {
        int diff = num.size() - m_number.size();
        for(int i = 0; i < diff; ++i)
        {
            m_number.push_front(0);
        }

        isBigger = true;
        main_start = rbegin(m_number);
        main_end = rend(m_number);
        second_start = rbegin(num);
        second_end = rend(num);
        std::cout << "handle\n";
    }

    for(; main_start != main_end; ++second_start, ++main_start)
    {
        if (*second_start < *main_start)
        {
            // TODO: remove to separate function
            *second_start += (10 - *main_start);
            if(isBigger) *main_start = *second_start;

            for(auto i = std::next(second_start, 1); i != second_end; ++i)
            {
                if(*i == 0) *i = 9;
                else
                {
                    *i -= 1;
                    break;
                }
            }

            std::prev(second_start, 1);
        }
        else
        {
            *second_start -= *main_start;
            if(isBigger) *main_start = *second_start;
        }
    }

    for(auto it = begin(m_number); it != end(m_number);)
    {
        if(*it == 0) it = m_number.erase(it);
        else
        {
            ++it;
            break;
        }
    }

    return *this;
}

namespace
{ // https://www.ideone.com/w8YSma
using comp = std::complex < double >;
using vcomp = std::vector < comp >;
using sint = int;
using vsint  = std::vector < sint >;

class Mult
{
    int reverse_bits (int a, int n)
    {
        int res = 0;
        for (int i = 0; i < n; ++ i)
        {
            res <<= 1;
            res |= a & 1;
            a >>= 1;
        }
        return res;
    }
    int logn (int a)
    {
        int cnt = 0;
        do
        {
            ++ cnt;
            a >>= 1;
        }
        while (a);
        return cnt;
    }
    int topow2 (int a)
    {
        for (int i = 0; i < 32; ++ i)
            if ((1 << i) >= a)
                return 1 << i;
        return - 1;
    }
    void swap_revbits (vcomp & a)
    {
        int l = logn (a.size ()) - 1;
        vcomp res (a.size ());
        for (std::size_t i = 0; i < a.size  (); ++ i)
            res[i] = a[reverse_bits (i, l)];
        a = res;
    }
    void fft (vcomp & a, bool back)
    {
        swap_revbits (a);
        int n = a.size ();
        double t = (back ? - 1 : 1);

        for (int m = 2; m <= n; m *= 2)
        {
            comp wm (cos (t * 2 * M_PI / (double)m), sin (t * 2 * M_PI / (double)m));

            for (int k = 0; k < n; k += m)
            {
                comp w (1);
                for (int j = 0; j < m / 2; ++ j)
                {
                    comp a0 = a[k + j];
                    comp w_a1 = w * a[k + j + m / 2];
                    a[k + j] = a0 + w_a1;
                    a[k + j + m / 2] = a0 - w_a1;

                    if (back)
                    {
                        a[k + j] /= 2.0;
                        a[k + j + m / 2] /= 2.0;
                    }
                    w *= wm;
                }
            }
        }
    }

public:
    vsint fft_mul (vsint a, vsint b)
    {
        int n = topow2 (std::max (a.size (), b.size ())) * 2;
        a.resize (n);
        b.resize (n);

        vcomp ac (a.begin (), a.end ());
        vcomp bc (b.begin (), b.end ());

        fft (ac, false);
        fft (bc, false);

        vcomp cc (n);
        for (int i = 0; i < n; ++ i)
            cc[i] = ac[i] * bc[i];
        fft (cc, true);

        vsint c (n, 0);
        for (int i = 0; i < n; ++ i)
            c[i] += (int)(cc[i].real () + 0.5);
        for (int i = 0; i < n - 1; ++ i)
            if (c[i] > 9)
            {
                c[i + 1] += c[i] / 10;
                c[i] %= 10;
            }
        while (c[c.size () - 1] > 9)
        {
            c.push_back (c[c.size () - 1] / 10);
            c[c.size () - 2] %= 10;
        }

        while (c.back () == 0 && c.size () > 1)
            c.pop_back ();

        return c;
    }

};
} // namespace

BigNumber::BigNumber BigNumber::BigNumber::operator*(BigNumber number)
{
    Mult mult;
    m_number.reverse();
    auto l = number.toVector();
    std::reverse(begin(l), end(l));
    auto res = mult.fft_mul(this->toVector(), l);

    std::list<int> resList;

    for(auto it = rbegin(res); it != rend(res); ++it)
    {
        resList.push_front(*it);
    }

    return BigNumber(resList);

}

BigNumber::BigNumber BigNumber::BigNumber::operator/(BigNumber number)
{
    if(m_number.size() < number.toList().size() || m_number < number.toList())
    {
        return BigNumber(0);
    }


//    static unsigned int find_first_bit(uint32_t value) {
//    #   ifdef _MSC_VER
//        unsigned long index;
//        (void) _BitScanReverse(&index, value);
//        return index + 1;
//    #   else
//        unsigned int count = 0;
//        for(count = 0; value; ++count)
//            value >>= 1;
//        return count;
//    #   endif
//    }


//    uint32_t divide(const uint32_t *num, const uint32_t *den, size_t len) {
//        uint32_t norm_den;
//        uint64_t norm_num = 0;
//        size_t top = 0;
//        while(norm_den = den[top], !norm_den)
//            norm_num = num[top++];
//        assert(len >= top + 3);

//        unsigned int norm = find_first_bit(norm_den);
//        norm_num = norm_num << (64 - norm);
//        norm_num |= ((uint64_t) num[top + 0] << 32 | num[top + 1]) >> norm;
//        norm_den = ((uint64_t) norm_den << 32 | den[top + 1]) >> norm;

//        uint32_t quot = (uint32_t) (norm_num / norm_den);

//        uint32_t rem = norm_num % norm_den;

//        norm_num = ((uint64_t) num[top + 1] << 32 | num[top + 2]) >> norm;
//        norm_num = (uint64_t) rem << 32 | (uint32_t) norm_num;
//        norm_den = ((uint64_t) den[top + 1] << 32 | den[top + 2]) >> norm;
//        if((uint64_t) quot * norm_den > norm_num) {
//            --quot;

//        }

//        uint64_t accum = 0;
//        do {
//            uint64_t prod = (uint64_t) quot * *den++;
//            accum = accum << 32 | *num++;

//            if(accum < prod)
//                return --quot;
//            accum -= prod;

//            if((uint64_t) accum >= 0x100000000)
//                return quot;
//        } while(--len);
//        return quot;
//    }

//    int main()
//    {
//        const uint32_t a[] = {824433244, 321654323, 000000000};
//        const uint32_t b[] = {243324432, 142350000, 000000000};

//        auto start = high_resolution_clock::now();
//        std::cout << divide(a, b, 3) << std::endl;
//        auto stop = high_resolution_clock::now();

//        auto duration = duration_cast<microseconds>(stop - start);

//        cout << duration.count() << endl;

//        return 0;
//    }

    return *this;
}

bool BigNumber::BigNumber::operator<(BigNumber other)
{
    if (m_number.size() < other.toList().size()) return true;
    else if (m_number.size() > other.toList().size()) return false;

    auto it = this->toList();
    auto it_other = other.toList();

    for(std::size_t i = 0; i < it.size(); ++i)
    {
        auto first_digit = std::next(it.begin(), i);
        auto second_digit = std::next(it_other.begin(), i);

        if(*first_digit < *second_digit) return true;
        if(*first_digit > *second_digit) return false;
    }

    return false;
}

bool BigNumber::BigNumber::operator>(BigNumber other)
{
    if (m_number.size() > other.toList().size()) return true;
    else if (m_number.size() < other.toList().size()) return false;

    auto it = this->toList();
    auto it_other = other.toList();

    for(std::size_t i = 0; i < it.size(); ++i)
    {
        auto first_digit = std::next(it.begin(), i);
        auto second_digit = std::next(it_other.begin(), i);

        if(*first_digit > *second_digit) return true;
        if(*first_digit < *second_digit) return false;
    }

    return false;
}

bool BigNumber::BigNumber::operator==(BigNumber other)
{
    if(m_number.size() != other.toList().size()) return false;

    auto it = this->toList();
    auto it_other = other.toList();

    for(std::size_t i = 0; i < it.size(); ++i)
    {
        auto first_digit = std::next(it.begin(), i);
        auto second_digit = std::next(it_other.begin(), i);

        if(*first_digit != *second_digit) return false;
    }

    return true;
}

bool BigNumber::BigNumber::operator!=(BigNumber other)
{
    return !(*this == other);
}

BigNumber::BigNumber& BigNumber::BigNumber::operator+(int number)
{
    BigNumber a(number);
    return *this + a;
}

BigNumber::BigNumber& BigNumber::BigNumber::operator+(BigNumber number)
{
    auto num = number.toList();

    if(m_number.size() < num.size())
    {
        int diff = num.size() - m_number.size();
        for(int i = 0; i < diff; ++i)
        {
            m_number.push_front(0);
        }
    }

    auto it = rbegin(m_number);
    auto it_num = rbegin(num);

    for(; it_num != rend(num); ++it, ++it_num)
    {
        // TODO: remove to separate function
        int digit = *it + *it_num;
        if(digit > 9)
        {
            *it = digit - 10;
            *(std::next(it, 1)) += 1;
            int count = 0;
            for(auto i = it; i != rend(m_number); ++i)
            {
                if(*i > 9)
                {
                    ++count;
                    *i = digit - 10;
                    *(std::next(i, 1)) += 1;
                }
                else break;
            }
            std::prev(it, count+1);
        }
        else *it = digit;
    }

    return *this;
}

std::string BigNumber::BigNumber::toString() const
{
    std::string number(m_number.size(), '0');

    int i = 0;
    for(const auto& digit : m_number)
    {
        number[i] = digit + '0';
        ++i;
    }

    return number;
}

std::list<int> BigNumber::BigNumber::toList() const
{
    return m_number;
}

std::vector<int> BigNumber::BigNumber::toVector() const
{
    std::vector<int> result;
    result.reserve(m_number.size());

    for(const auto& digit : m_number)
    {
        result.push_back(digit);
    }

    return result;
}

std::deque<int> BigNumber::BigNumber::toDeque() const
{
    std::deque<int> result;

    for(const auto& digit : m_number)
    {
        result.push_back(digit);
    }

    return result;
}
