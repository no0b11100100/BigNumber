#pragma once
#include <cmath>
#include <iterator>
#include "Converter.hpp"
#include "Operators.hpp"
#include <ostream>
#include <iostream>

namespace BigInt
{

class BigInt final
{
    template<class T, class Callback>
    static BinaryReturnType process (const T& smaller, const T& bigger, Callback&& callback)
    {
        size_t bits{0};
        BinaryData result(bigger.size());
        std::transform(smaller.crbegin(), smaller.crend(), bigger.crbegin(), result.rbegin(), [&](const Bit& lhs, const Bit& rhs)
        {
            Bit bit = callback(lhs, rhs);
            if(bit == 1) ++bits;
            return bit;
        });

        size_t size = std::distance(smaller.crbegin(), smaller.crend());
        std::transform(std::next(bigger.crbegin(), size), bigger.crend(), std::next(result.rbegin(), size), [&](const Bit& bit)
        {
            if(bit == 1) ++bits;
            return bit;
        });

        removeInsignificantBits(result);
        return {result, bits};
    }

    template<class T, class Callback>
    static BinaryReturnType conditionForProcess (const T& lhs, const T& rhs, Callback&& callback)
    {
        return lhs.bit() <= rhs.bit() ?
                        process(lhs.Number(), rhs.Number(), callback) :
                        process(rhs.Number(), lhs.Number(), callback);
    }

    template<class Callback>
    static BigInt Transform(const BigInt& lhs, const BigInt& rhs, Callback&& callback)
    {
        auto[result, bits] = conditionForProcess(lhs.isNegative() ? ~lhs : lhs,
                                                 rhs.isNegative() ? ~rhs : rhs, callback);
        return lhs.sign() == rhs.sign() ? BigInt(result, bits) : ~(BigInt(result, bits));
    }

    State m_state;

    BigInt(const BinaryData& number, size_t bits = 0, Sign sign = Sign::Positive):
        m_state(number, bits, sign)
    {}

    BigInt(const State& state):
        m_state{state}
    {}

public:
    BigInt():
        m_state{State()}
    {}

    BigInt(const std::string value) :
        m_state{ ToBinary::convert( value ) }
    {}

    template<class T, class = typename std::enable_if_t<is_integer<T>()>>
    BigInt(T value)
        : m_state{ ToBinary::convert( value ) }
    {}

    inline const BinaryData& Number() const { return m_state.number; }
    inline size_t count() const { return m_state.bitSet; }
    inline size_t bit() const { return m_state.number.size(); }
    inline Sign sign() const { return m_state.sign; }
    inline bool is2Pow() const { return m_state.bitSet == 1; }
    bool isPrime() const
    {
        bool notPrime = bit() == 1 && (isZero() || isUnit()); // 0,1
        bool prime = !notPrime && bit() == 2 &&
                ( equal(BinaryData({1,0}), Number()) || equal(BinaryData({1,1}), Number())) ;
        if(notPrime) return false;
        if(prime) return true;

        if ( ( (Pow(*this, 2)) % BigInt(BinaryData({1,1,0,0,0}), 2) ).isUnit())
            return true;

        return false;
    }
    inline bool isPositive() const { return m_state.sign == Sign::Positive; }
    inline bool isNegative() const { return !isPositive(); }
    inline bool isEven() const { return *Number().rbegin() == 0; }
    inline bool isOdd() const { return !isEven(); }
    inline bool isZero() const { return bit() == 1 && *Number().begin() == 0; }
    inline bool isUnit() const { return bit() == 1 && *Number().begin() == 1; }
    inline void MakePositive() { m_state.sign = Sign::Positive; }
    inline void MakeNegative() { m_state.sign = Sign::Negative; }
    inline std::string toBinary() const { return FromBinary::ToBinary(m_state.number, m_state.sign); }
    inline std::string toOctal() const
    {
        return isNegative() ? '-' + FromBinary::ToOctal(m_state.number)
                            : FromBinary::ToOctal(m_state.number);
    }
    inline std::string toDecimal() const
    {
        return isNegative() ? '-' + FromBinary::ToDecimal(m_state.number)
                            : FromBinary::ToDecimal(m_state.number);
    }
    inline std::string toHex() const
    {
        return  isNegative() ? '-' + FromBinary::ToHex(m_state.number)
                             : FromBinary::ToHex(m_state.number);
    }

    friend BigInt operator + (const BigInt& lhs, const BigInt& rhs)
    {
        if( (lhs.isPositive() && rhs.isPositive()) ||
                (lhs.isNegative() && rhs.isNegative()) )
        {
            auto [result, bits] = addition(lhs.Number(), rhs.Number());
            return BigInt(result, bits, Sign::Positive);
        }
        if(lhs.isNegative())
        {
            if(less(lhs.Number(), rhs.Number()))
            {
                auto [result, bits] = subtraction(lhs.Number(), rhs.Number());
                return BigInt(result, bits, Sign::Positive);
            }
            else
            {
                auto [result, bits] = subtraction(rhs.Number(), lhs.Number());
                return BigInt(result, bits, Sign::Negative);
            }
        } else if(rhs.isNegative())
        {
            if(less(rhs.Number(), lhs.Number()))
            {
                auto [result, bits] = subtraction(lhs.Number(), rhs.Number());
                return BigInt(result, bits, Sign::Positive);
            }
            else
            {
                auto [result, bits] = subtraction(rhs.Number(), lhs.Number());
                return BigInt(result, bits, Sign::Negative);
            }
        }
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator + (const BigInt& lhs, T rhs)
    {
        return operator+(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator + (T lhs, const BigInt& rhs)
    {
        return operator+(BigInt(lhs), rhs);
    }

    BigInt& operator += (const BigInt& other)
    {
        *this = operator+(*this, other);
        return *this;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    BigInt& operator += (T other)
    {
        *this = operator+(*this, BigInt(other));
        return *this;
    }

    friend BigInt operator - (const BigInt& lhs, const BigInt& rhs)
    {
        if(lhs.isPositive() && rhs.isNegative())
        {
            auto [result, bits] = addition(lhs.Number(), rhs.Number());
            return BigInt(result, bits, Sign::Positive);
        }
        else if(lhs.isNegative() && rhs.isPositive())
        {
            auto [result, bits] = addition(lhs.Number(), rhs.Number());
            return BigInt(result, bits, Sign::Negative);
        }

        bool isBigger = greater(lhs.Number(), rhs.Number());
        Sign sign = (lhs.isNegative() && rhs.isNegative()) ?
                    (isBigger ? Sign::Negative : Sign::Positive) :
                    (isBigger ? Sign::Positive : Sign::Negative);
        auto [result, bits] = isBigger ?
                subtraction(lhs.Number(), rhs.Number()) :
                subtraction(rhs.Number(), lhs.Number());

        return BigInt(result, bits, sign);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator - (const BigInt& lhs, T rhs)
    {
        return operator-(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator - (T lhs, const BigInt& rhs)
    {
        return operator-(BigInt(lhs), rhs);
    }

    BigInt& operator -= (const BigInt& other)
    {
        *this = operator-(*this, other);
        return *this;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    BigInt& operator -= (T other)
    {
        *this = operator-(*this, BigInt(toBinary(other)));
        return *this;
    }

    friend BigInt operator * (const BigInt& lhs, const BigInt& rhs)
    {
        if(lhs.isZero() || rhs.isZero()) return BigInt();
        Sign sign = lhs.sign() == rhs.sign() ? Sign::Positive : Sign::Negative;
        if(lhs.isUnit()) return rhs;
        if(rhs.isUnit()) return lhs;
        if(lhs.is2Pow()) return rhs << lhs.bit();
        if(rhs.is2Pow()) return lhs << rhs.bit();

        auto[result, bits] = multiplication(lhs.Number(), rhs.Number());
        return BigInt(result, bits, sign);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator * (const BigInt& lhs, T rhs)
    {
        return operator*(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator * (T lhs, const BigInt& rhs)
    {
        return operator*(BigInt(lhs), rhs);
    }

    BigInt& operator *= (const BigInt& other)
    {
        *this = operator*(*this, other);
        return *this;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    BigInt& operator *= (T other)
    {
        *this = operator*(*this, BigInt(toBinary(other)));
        return *this;
    }

    friend BigInt operator / (const BigInt& lhs, const BigInt& rhs)
    {
        if(rhs.isZero()) throw "Division by zero";
        if(lhs < rhs) return BigInt();
        if(rhs.isUnit()) return lhs;
        if(rhs.is2Pow()) return lhs >> rhs.bit();

        Sign sign = lhs.sign() == rhs.sign() ? Sign::Positive : Sign::Negative;
        auto [result, bits] = division(lhs.Number(), rhs.Number(), Division::Mode::Div);
        return BigInt(result, bits, sign);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator / (const BigInt& lhs, T rhs)
    {
        return operator/(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator / (T lhs, const BigInt& rhs)
    {
        return operator/(BigInt(lhs), rhs);
    }

    BigInt& operator /= (const BigInt& other)
    {
        *this = operator/(*this, other);
        return *this;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    BigInt& operator /= (T other)
    {
        *this = operator/(*this, BigInt(toBinary(other)));
        return *this;
    }

    friend BigInt operator % (const BigInt& lhs, const BigInt& rhs)
    {
        if(lhs < rhs) return lhs;
        Sign sign = lhs.isNegative() ? Sign::Negative : Sign::Positive;
        auto [result, bits] = division(lhs.Number(), rhs.Number(), Division::Mode::Mod);
        return BigInt(result, bits, sign);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator % (const BigInt& lhs, T rhs)
    {
        return operator%(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator % (T lhs, const BigInt& rhs)
    {
        return operator%(BigInt(lhs), rhs);
    }

    BigInt& operator %= (const BigInt& other)
    {
        *this = operator%(*this, other);
        return *this;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    BigInt& operator %= (T other)
    {
        *this = operator%(*this, BigInt(toBinary(other)));
        return *this;
    }

    friend bool operator < (const BigInt& lhs, const BigInt& rhs)
    {
        if(lhs.isPositive() && rhs.isNegative()) return false;
        if(lhs.isNegative() && rhs.isPositive()) return true;
        if(lhs.isNegative() && rhs.isNegative()) return greater(lhs.Number(), rhs.Number());
        return less(lhs.Number(), rhs.Number());
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator < (const BigInt& lhs, T rhs)
    {
        return operator <(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator < (T lhs, const BigInt& rhs)
    {
        return operator <(BigInt(lhs), rhs);
    }

    friend bool operator <= (const BigInt& lhs, const BigInt& rhs)
    {
        return lessOrEqual(lhs.Number(), rhs.Number());
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator <= (const BigInt& lhs, T rhs)
    {
        return operator <=(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator <= (T lhs, const BigInt& rhs)
    {
        return operator <=(BigInt(lhs), rhs);
    }

    friend bool operator > (const BigInt& lhs, const BigInt& rhs)
    {
        if(lhs.isPositive() && rhs.isNegative()) return true;
        if(lhs.isNegative() && rhs.isPositive()) return false;
        if(lhs.isNegative() && rhs.isNegative()) return less(lhs.Number(), rhs.Number());
        return greater(lhs.Number(), rhs.Number());
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator > (const BigInt& lhs, T rhs)
    {
        return operator >(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator > (T lhs, const BigInt& rhs)
    {
        return operator >(BigInt(lhs), rhs);
    }

    friend bool operator >= (const BigInt& lhs, const BigInt& rhs)
    {
        return greatOrEqual(lhs.Number(), rhs.Number());
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator >= (const BigInt& lhs, T rhs)
    {
        return operator >=(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator >= (T lhs, const BigInt& rhs)
    {
        return operator >=(BigInt(lhs), rhs);
    }

    friend bool operator == (const BigInt& lhs, const BigInt& rhs)
    {
        if(lhs.sign() != rhs.sign()) return false;
        return equal(lhs.Number(), rhs.Number());
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator == (const BigInt& lhs, T rhs)
    {
        return operator ==(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator == (T lhs, const BigInt& rhs)
    {
        return operator ==(BigInt(lhs), rhs);
    }

    friend bool operator != (const BigInt& lhs, const BigInt& rhs)
    {
        return !operator ==(lhs, rhs);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator != (const BigInt& lhs, T rhs)
    {
        return operator !=(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator != (T lhs, const BigInt& rhs)
    {
        return operator !=(BigInt(lhs), rhs);
    }

    friend BigInt operator ^ (const BigInt& lhs, const BigInt& rhs)
    {
        return Transform(lhs, rhs, predicates['^']);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator ^ (const BigInt& lhs, T rhs)
    {
        return operator^(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator ^ (T lhs, const BigInt& rhs)
    {
        return operator^(BigInt(lhs), rhs);
    }

    BigInt& operator ^= (const BigInt& other)
    {
        *this = Transform(*this, other, predicates['^']);
        return *this;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    BigInt& operator ^= (T other)
    {
        return operator^=(BigInt(toBinary(other)));
    }

    friend BigInt operator | (const BigInt& lhs, const BigInt& rhs)
    {
        return Transform(lhs, rhs, predicates['|']);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator | (const BigInt& lhs, T rhs)
    {
        return operator|(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator | (T lhs, const BigInt& rhs)
    {
        return operator|(BigInt(lhs), rhs);
    }

    BigInt& operator |= (const BigInt& other)
    {
        *this = Transform(*this, other, predicates['|']);
        return *this;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    BigInt& operator |= (T other)
    {
        return operator|=(BigInt(toBinary(other)));
    }

    friend BigInt operator & (const BigInt& lhs, const BigInt& rhs)
    {
        return Transform(lhs, rhs, predicates['&']);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator & (const BigInt& lhs, T rhs)
    {
        return operator&(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator & (T lhs, const BigInt& rhs)
    {
        return operator&(BigInt(lhs), rhs);
    }

    BigInt& operator &= (const BigInt& other)
    {
        *this = Transform(*this, other, predicates['&']);
        return *this;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    BigInt& operator &= (T other)
    {
        return operator&=(BigInt(toBinary(other)));
    }

    friend BigInt operator ~(const BigInt& number)
    {
        if(number.isZero())
            return BigInt(BinaryData{1}, 1, Sign::Negative);

        if(number.isPositive())
        {
            BinaryData data = number.Number();
            size_t bits = number.count();
            Increment(data, bits);
            return BigInt(data, bits, Sign::Negative);
        }
        else
        {
            BinaryData data = number.Number();
            size_t bits = number.count();
            Decrement(data, bits);
            return BigInt(data, bits, Sign::Positive);
        }

        return BigInt();
    }

    friend BigInt operator -(const BigInt& number)
    {
        return BigInt(number.Number(), number.count(), number.isPositive() ? Sign::Negative : Sign::Negative);
    }

    BigInt operator !()
    {
        return isZero() ? BigInt(BinaryData{1}, 1, Sign::Positive) : BigInt();
    }

    friend BigInt operator << (const BigInt& lhs, const size_t offset)
    {
        BinaryData newResult = lhs.Number();
        leftShift(newResult, offset);
        return BigInt(newResult, lhs.count(), lhs.sign());
    }

    BigInt& operator <<= (const size_t offset)
    {
        leftShift(m_state.number, offset);
        return *this;
    }

    friend BigInt operator >> (const BigInt& lhs, const size_t offset)
    {
        BinaryData newResult = lhs.Number();
        size_t units = lhs.count();
        rightShift(newResult, units, offset);
        return BigInt(newResult, units, lhs.sign());
    }

    BigInt& operator >>= (const size_t offset)
    {
        rightShift(m_state.number, m_state.bitSet, offset);
        return *this;
    }

    friend std::ostream& operator << (std::ostream& os, const BigInt& number)
    {
        os << number.toDecimal();
        return os;
    }

    friend std::istream& operator >> (std::istream& is, BigInt& number)
    {
        std::string str;
        is >> str;
        number = str;
        return is;
    }

    BigInt& operator++()
    {
        if(isNegative())
            Decrement(m_state.number, m_state.bitSet);
        Increment(m_state.number, m_state.bitSet);
        return *this;
    }

    BigInt& operator--()
    {
        if(isZero())
            *this = BigInt(BinaryData{1}, 1, Sign::Negative);
        else if(isPositive())
            Decrement(m_state.number, m_state.bitSet);
        else if(isNegative())
            Increment(m_state.number, m_state.bitSet);

        return *this;
    }

    BigInt operator++(int)
    {
        auto tmp = *this;
        if(isNegative())
            Decrement(m_state.number, m_state.bitSet);
        Increment(m_state.number, m_state.bitSet);
        return tmp;
    }

    BigInt operator--(int)
    {
        auto tmp = *this;
        if(isZero())
            *this = BigInt(BinaryData{1}, 1, Sign::Negative);
        else if(isPositive())
            Decrement(m_state.number, m_state.bitSet);
        else if(isNegative())
            Increment(m_state.number, m_state.bitSet);

        return tmp;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>()>>
    explicit operator T()
    {
        T limit = isNegative() ? std::numeric_limits<T>::min()
                                    : std::numeric_limits<T>::max();

        if(ToBinary::convert(limit).number.size() <= bit())
            return isNegative() ? -1*FromBinary::toInt<T>(Number())
                              : FromBinary::toInt<T>(Number());

        return 0;
    }

    static BigInt abs(const BigInt& number)
    {
        return number.isNegative() ? BigInt(number.Number(), number.bit(), Sign::Positive)
                                   : number;
    }

    static BigInt Pow(const BigInt& number, size_t pow)
    {
        static auto countBits = [](const size_t& number) -> size_t
        {
            return static_cast<size_t>(log2(number));
        };

        BigInt result(1);
        BigInt tempResult = number;

        while(pow)
        {
            if(pow == 1)
            {
                result *= number;
                break;
            }
            size_t count = countBits(pow);
            for(size_t i{0}; i < count; ++i)
                tempResult *= tempResult;

            pow -= 1 << count;
            result *= tempResult;
            tempResult = number;
        }

        return result;
    }

    static std::vector<BigInt> factorize(BigInt number)
    {
        std::vector<BigInt> factors;
        for(BigInt i(BinaryData({1,0}), 1); i <= Sqrt(number); ++i)
        {
            while((number%i).isZero())
            {
                factors.push_back(i);
                number /= i;
            }
        }

        if(number.isUnit()) factors.push_back(number);
        return factors;
    }

    static BigInt fibonacci(std::size_t nElem)
    {
        BigInt a(BinaryData(1), 1), b(BinaryData(1), 1), c(BinaryData(1), 1), rd(BinaryData(1), 1);
        BigInt ta, tb, rc, tc, d;

        while(nElem)
        {
            if(nElem&1) // isOdd
            {
                tc = rc;
                rc = rc*a + rd*c;
                rd = tc*b + rd*d;
            }

            ta = a;
            tb = b;
            tc = c;
            a = a*a + b*c;
            b = ta*b + b*d;
            c = c*ta + d*c;
            d = tc*tb+ d*d;

            nElem /= 2;
        }

        return rc;
    }

    static BigInt factorial(std::size_t nElem)
    {
        if(nElem == 1 || nElem == 0)
            return BigInt(BinaryData(1), 1, Sign::Positive);

        bool handleOdd {false};
        size_t uptoNumber {nElem};

        if((nElem & 1) == 1)
        {
            --uptoNumber;
            handleOdd = true;
        }

        BigInt nextSum(uptoNumber);
        BigInt nextMulti(uptoNumber);
        BigInt factorial(BinaryData(1), 1);

        while(nextSum >= 2)
        {
            factorial *= nextMulti;
            nextSum -= 2;
            nextMulti += nextSum;
        }

        if (handleOdd) factorial *= nElem;

        return factorial;
    }

    static BigInt gcd(const BigInt& u, const BigInt& v)
    {
        if (u == v) return u;
        if (u.isZero()) return v;
        if (v.isZero()) return u;

        if (u.isEven())
        {
            if (v.isOdd()) return gcd(u>>1, v);
            else return gcd(u>>1, v>>1) << 1;
        }
        else
        {
            if (v.isEven()) return gcd(u, v>>1);
            if (u > v) return gcd((u - v)>>1, v);
            else return gcd((v - u)>>1, u);
        }
    }

    static BigInt lcm(const BigInt& a, const BigInt& b)
    {
        return (a*b) / gcd(a,b);
    }

    static BigInt Sqrt(BigInt number)
    {
        if(number.isNegative() || number.isZero()) return BigInt();
        BigInt result;
        const short roundingToLowPow2 = 2;
        BigInt bit = 1 << (number.bit()-roundingToLowPow2);

        while (!bit.isZero())
        {
            if(auto add = result + bit; number >= add)
            {
                number -= add;
                result = (result>>1) + bit;
            }
            else result >>= 1;

            bit >>= 2;
        }

        return result;
    }
};

} // namespace BigInt
