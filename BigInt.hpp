#pragma once
#include <cmath>
#include <iterator>
#include "Converter.hpp"
#include "Operators.hpp"

namespace BigInt
{

namespace
{

std::unordered_map<char, std::function<Bit(const Bit&, const Bit&)>> predicates
{
    { '^', [](const Bit& lhsBit, const Bit& rhsBit) { return lhsBit == rhsBit ?      0 : 1; } },
    { '|', [](const Bit& lhsBit, const Bit& rhsBit) { return lhsBit == rhsBit ? lhsBit : 1; } },
    { '&', [](const Bit& lhsBit, const Bit& rhsBit) { return lhsBit == rhsBit ? lhsBit : 0; } },
};

}// namespace

// TODO: make operators with int
class BigInt final
{
    template<class Predicate>
    static BigInt Transform(const BigInt& lhs, const BigInt& rhs, Predicate predicate)
    {
        BinaryData result;
        static auto forEach = [&](const Bit& bit) { result.push_front(bit); };
        if(lhs.bit() < rhs.bit())
        {
            std::transform(lhs.Number().crbegin(), lhs.Number().crend(), rhs.Number().crbegin(), std::front_inserter(result), predicate);
            auto it = std::next( rhs.Number().crbegin(), lhs.bit() );
            std::for_each(it, rhs.Number().crend(), forEach);
        }
        else if(lhs.bit() > rhs.bit())
        {
            std::transform(rhs.Number().crbegin(), rhs.Number().crend(), lhs.Number().crbegin(), std::front_inserter(result), predicate);
            auto it = std::next( lhs.Number().crbegin(), rhs.bit() );
            std::for_each(it, lhs.Number().crend(), forEach);
        }
        else
            std::transform(lhs.Number().crbegin(), lhs.Number().crend(), rhs.Number().crbegin(), std::front_inserter(result), predicate);

        return BigInt(result, 0, Sign::Positive); // TODO
    }

    template<class Predicate>
    static void Transform(BinaryData& number, const BigInt& other, Predicate predicate)
    {
        if(number.size() <= other.bit())
        {
            std::transform(number.crbegin(), number.crend(), other.Number().crbegin(), number.rbegin(), predicate);
            auto it = std::next( other.Number().crbegin(), number.size() );
            std::for_each(it, other.Number().crend(), [&](const Bit& bit) { number.push_front(bit); });
        }
        else
            std::transform(other.Number().crbegin(), other.Number().crend(), number.crbegin(), number.rbegin(), predicate);

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

    template<class T>
    BigInt(T&& value)
        : m_state{toBinary(std::forward<T>(value))}
    {}

    const BinaryData& Number() const { return m_state.number; }
    size_t count() const { return m_state.bitSet; }
    size_t bit() const { return m_state.number.size(); }
    Sign sign() const { return m_state.sign; }
    bool is2Pow() const { return m_state.bitSet == 1; }
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
    bool isPositive() const { return m_state.sign == Sign::Positive; }
    bool isNegative() const { return isPositive(); }
    bool isEven() const { return *Number().rbegin() == 0; }
    bool isOdd() const { return !isEven(); }
    bool isZero() const { return bit() == 1 && *Number().begin() == 0; }
    bool isUnit() const { return bit() == 1 && *Number().begin() == 1; }
    void MakePositive() { m_state.sign = Sign::Positive; }
    void MakeNegative() { m_state.sign = Sign::Negative; }

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
        return operator+(lhs, BigInt(toBinary(rhs)));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator + (T lhs, const BigInt& rhs)
    {
        return operator+(BigInt(toBinary(lhs)), rhs);
    }

    BigInt& operator += (const BigInt& other)
    {
        *this = operator+(*this, other);
        return *this;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    BigInt& operator += (T other)
    {
        *this = operator+(*this, BigInt(toBinary(other)));
        return *this;
    }

    friend BigInt operator - (const BigInt& lhs, const BigInt& rhs)
    {
        if(lhs.isPositive() && rhs.isPositive())
        {
            auto [result, bits] = addition(lhs.Number(), rhs.Number());
            return BigInt(result, bits, Sign::Positive);
        }

        auto [result, bits] =  lhs > rhs ?
                subtraction(lhs.Number(), rhs.Number()) :
                subtraction(rhs.Number(), lhs.Number());

        return BigInt(result, bits, Sign::Negative);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator - (const BigInt& lhs, T rhs)
    {
        return operator-(lhs, BigInt(toBinary(rhs)));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator - (T lhs, const BigInt& rhs)
    {
        return operator-(BigInt(toBinary(lhs)), rhs);
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
        Sign sign = ( (lhs.isPositive() && rhs.isPositive()) ||
                      (lhs.isNegative() && rhs.isNegative())) ? Sign::Positive :
                                                                Sign::Negative;
        if(lhs.is2Pow()) return rhs << lhs.bit();
        if(rhs.is2Pow()) return lhs << rhs.bit();

        auto[result, bits] = multiplication(lhs.Number(), rhs.Number());
        return BigInt(result, bits, sign);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator * (const BigInt& lhs, T rhs)
    {
        return operator*(lhs, BigInt(toBinary(rhs)));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator * (T lhs, const BigInt& rhs)
    {
        return operator*(BigInt(toBinary(lhs)), rhs);
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
        if(lhs < rhs) return BigInt();        
        if(rhs.is2Pow()) return lhs >> rhs.bit();

        Sign sign = ( (lhs.isPositive() && rhs.isPositive()) ||
                      (lhs.isNegative() && rhs.isNegative())) ? Sign::Positive :
                                                                Sign::Negative;
        auto [result, bits] = division(lhs.Number(), rhs.Number(), Division::Mode::Div);
        return BigInt(result, bits, sign);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator / (const BigInt& lhs, T rhs)
    {
        return operator/(lhs, BigInt(toBinary(rhs)));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator / (T lhs, const BigInt& rhs)
    {
        return operator/(BigInt(toBinary(lhs)), rhs);
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
        return operator%(lhs, BigInt(toBinary(rhs)));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator % (T lhs, const BigInt& rhs)
    {
        return operator%(BigInt(toBinary(lhs)), rhs);
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
        return operator <(lhs, BigInt(toBinary(rhs)));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator < (T lhs, const BigInt& rhs)
    {
        return operator <(BigInt(toBinary(lhs)), rhs);
    }

    friend bool operator <= (const BigInt& lhs, const BigInt& rhs)
    {
        return greater(rhs.Number(), lhs.Number());
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator <= (const BigInt& lhs, T rhs)
    {
        return operator <=(lhs, BigInt(toBinary(rhs)));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator <= (T lhs, const BigInt& rhs)
    {
        return operator <=(BigInt(toBinary(lhs)), rhs);
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
        return operator >(lhs, BigInt(toBinary(rhs)));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator > (T lhs, const BigInt& rhs)
    {
        return operator >(BigInt(toBinary(lhs)), rhs);
    }

    friend bool operator >= (const BigInt& lhs, const BigInt& rhs)
    {
        return less(rhs.Number(), lhs.Number());
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator >= (const BigInt& lhs, T rhs)
    {
        return operator >=(lhs, BigInt(toBinary(rhs)));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator >= (T lhs, const BigInt& rhs)
    {
        return operator >=(BigInt(toBinary(lhs)), rhs);
    }

    friend bool operator == (const BigInt& lhs, const BigInt& rhs)
    {
        if(lhs.sign() != rhs.sign()) return false;
        return equal(lhs.Number(), rhs.Number());
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator == (const BigInt& lhs, T rhs)
    {
        return operator ==(lhs, BigInt(toBinary(rhs)));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator == (T lhs, const BigInt& rhs)
    {
        return operator ==(BigInt(toBinary(lhs)), rhs);
    }

    friend bool operator != (const BigInt& lhs, const BigInt& rhs)
    {
        return !operator ==(lhs, rhs);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator != (const BigInt& lhs, T rhs)
    {
        return operator !=(lhs, BigInt(toBinary(rhs)));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator != (T lhs, const BigInt& rhs)
    {
        return operator !=(BigInt(toBinary(lhs)), rhs);
    }

    friend BigInt operator ^ (const BigInt& lhs, const BigInt& rhs)
    {
        return Transform(lhs, rhs, predicates['^']);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator ^ (const BigInt& lhs, T rhs)
    {
        return operator^(lhs, BigInt(toBinary(rhs)));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator ^ (T lhs, const BigInt& rhs)
    {
        return operator^(BigInt(toBinary(lhs)), rhs);
    }

    BigInt& operator ^= (const BigInt& other)
    {
        Transform(m_state.number, other, predicates['^']);
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
        return operator|(lhs, BigInt(toBinary(rhs)));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator | (T lhs, const BigInt& rhs)
    {
        return operator|(BigInt(toBinary(lhs)), rhs);
    }

    BigInt& operator |= (const BigInt& other)
    {
        Transform(m_state.number, other, predicates['|']);
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
        return operator&(lhs, BigInt(toBinary(rhs)));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator & (T lhs, const BigInt& rhs)
    {
        return operator&(BigInt(toBinary(lhs)), rhs);
    }

    BigInt& operator &= (const BigInt& other)
    {
        Transform(m_state.number, other, predicates['&']);
        return *this;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    BigInt& operator &= (T other)
    {
        return operator&=(BigInt(toBinary(other)));
    }

    BigInt& operator ~()
    {
        if(isZero())
        {
            *this = BigInt(BinaryData{1}, 1, Sign::Negative);
            return *this;
        }

        if(isPositive())
        {
            m_state.sign = Sign::Negative;
            Increment(m_state.number, m_state.bitSet);
        }
        else
        {
            m_state.sign = Sign::Positive;
            Decrement(m_state.number, m_state.bitSet);
        }
        return *this;
    }

    BigInt& operator -()
    {
        m_state.sign = isPositive() ? Sign::Negative : Sign::Positive;
        return *this;
    }

    BigInt operator !()
    {
        return isZero() ? BigInt(BinaryData{1}, 1, Sign::Positive) : BigInt();
    }

    friend BigInt operator << (const BigInt& lhs, const size_t offset)
    {
        BinaryData newResult = lhs.Number();
        leftShift(newResult, offset);
        return BigInt(newResult, lhs.count(), Sign::Positive); // TODO: sign
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
        return BigInt(newResult, units, Sign::Positive); // TODO: count bits
    }

    BigInt& operator >>= (const size_t offset)
    {
        rightShift(m_state.number, m_state.bitSet, offset);
        return *this;
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

    static void abs(BigInt& number)
    {
        if(number.isNegative()) number.MakePositive();
    }

    static BigInt Pow(const BigInt& number, size_t pow)
    {
        BigInt result;
        static auto routingTo2Pow = [](size_t pow){
            pow--;
            pow |= pow >> 1;
            pow |= pow >> 2;
            pow |= pow >> 4;
            pow |= pow >> 8;
            pow |= pow >> 16;
            pow |= pow >> 32;
            return ++pow;
        };

        while(pow)
        {
            size_t routing = routingTo2Pow(pow);
            BigInt tmpRes = number;
            for(size_t i {0}; i < routing; ++i)
                tmpRes *= tmpRes;

            result += tmpRes;
            pow -= routing;
        }

        return result;
    }

    static std::vector<BigInt> factorize(BigInt number)
    {
        auto isOne = [&](const BinaryData& data)
                     { return data.size() == 1 && *data.begin() == 1; };

        std::vector<BigInt> factors;
        for(BigInt i(BinaryData({1,0})); i <= BigInt(square(number.Number())); ++i)
        {
            while((number%i).isZero())
            {
                factors.push_back(i);
                number /= i;
            }
        }

        if(isOne(number.Number())) factors.push_back(number);
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

        BinaryData two(2);

        if((nElem & 1) == 1)
        {
            --uptoNumber;
            handleOdd = true;
        }

        BigInt nextSum(uptoNumber);
        BigInt nextMulti(uptoNumber);
        BigInt factorial(BinaryData(1), 1);

        while(less(nextSum.Number(), two)) // nextSum >= 2
        {
            factorial *= nextMulti;
//            nextSum -= 2;
            nextMulti += nextSum;
        }

//        if (handleOdd) factorial *= nElem;

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

};

} // namespace BigInt
