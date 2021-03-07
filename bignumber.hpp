#pragma once
#include <cmath>
#include <iterator>
#include "Converter.hpp"
#include "Operators.hpp"

namespace BigInt
{

namespace
{

using predicateType =  std::function<Bit(const Bit&, const Bit&)>;
std::unordered_map<char, predicateType>predicates
{
    { '^', [](const Bit& lhsBit, const Bit& rhsBit) { return lhsBit == rhsBit ?      0 : 1; } },
    { '|', [](const Bit& lhsBit, const Bit& rhsBit) { return lhsBit == rhsBit ? lhsBit : 1; } },
    { '&', [](const Bit& lhsBit, const Bit& rhsBit) { return lhsBit == rhsBit ? lhsBit : 0; } },
};

}// namespace

class BigInt final
{
    enum class Sign : bool
    {
        Positive,
        Negative
    };

    template<class Predicate>
    static BigInt Transform(const BigInt& lhs, const BigInt& rhs, Predicate predicate)
    {
        BinaryData result;
        static auto forEach = [&](const Bit& bit) { result.push_front(bit); };
        if(lhs.count() < rhs.count())
        {
            std::transform(lhs.Number().crbegin(), lhs.Number().crend(), rhs.Number().crbegin(), std::front_inserter(result), predicate);
            auto it = std::next( rhs.Number().crbegin(), lhs.count() );
            std::for_each(it, rhs.Number().crend(), forEach);
        }
        else if(lhs.count() > rhs.count())
        {
            std::transform(rhs.Number().crbegin(), rhs.Number().crend(), lhs.Number().crbegin(), std::front_inserter(result), predicate);
            auto it = std::next( lhs.Number().crbegin(), rhs.count() );
            std::for_each(it, lhs.Number().crend(), forEach);
        }
        else
            std::transform(lhs.Number().crbegin(), lhs.Number().crend(), rhs.Number().crbegin(), std::front_inserter(result), predicate);

        return BigInt(result, 0, Sign::Positive); // TODO
    }

    template<class Predicate>
    static void Transform(BinaryData& number, const BigInt& other, Predicate predicate)
    {
        if(number.size() <= other.count())
        {
            std::transform(number.crbegin(), number.crend(), other.Number().crbegin(), number.rbegin(), predicate);
            auto it = std::next( other.Number().crbegin(), number.size() );
            std::for_each(it, other.Number().crend(), [&](const Bit& bit) { number.push_front(bit); });
        }
        else
            std::transform(other.Number().crbegin(), other.Number().crend(), number.crbegin(), number.rbegin(), predicate);

    }

    BinaryData m_number;
    std::size_t m_bitSet;
    Sign m_sign;

    BigInt(const BinaryData& number, size_t bits, Sign sign):
        m_number{number},
        m_bitSet{bits},
        m_sign{sign}
    {}

public:
    BigInt():
        m_number{0},
        m_bitSet{0},
        m_sign{Sign::Positive}
    {}

    template<class T>
    BigInt(T&& value, Base base)
        : m_number{toBinary(value, base)}
    {}

    const BinaryData& Number() const { return m_number; }
    size_t count() const { return m_bitSet; }
    bool isPositive() const { return m_sign == Sign::Positive; }
    bool isNegative() const { return isPositive(); }
    bool isEven() const { return *m_number.rbegin() == 0; }
    bool isOdd() const { return !isEven(); }
    bool isZero() const { return *m_number.begin() == 0; }
    void MakePositive() { m_sign = Sign::Positive; }
    void MakeNegative() { m_sign = Sign::Negative; }

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

    BigInt& operator += (const BigInt& other)
    {
        *this = operator+(*this, other);
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

    BigInt& operator -= (const BigInt& other)
    {
//        *this = operator-(*this, other);
        return *this;
    }

    friend BigInt operator * (const BigInt& lhs, const BigInt& rhs)
    {
        Sign sign = ( (lhs.isPositive() && rhs.isPositive()) ||
                      (lhs.isNegative() && rhs.isNegative())) ? Sign::Positive :
                                                                Sign::Negative;
        auto[result, bits] = multiplication(lhs.Number(), rhs.Number());
        return BigInt(result, bits, sign);
    }

    BigInt& operator *= (const BigInt& other)
    {
        *this = operator*(*this, other);
        return *this;
    }

    friend BigInt operator / (const BigInt& lhs, const BigInt& rhs)
    {
        if(lhs < rhs) return BigInt();
        Sign sign = ( (lhs.isPositive() && rhs.isPositive()) ||
                      (lhs.isNegative() && rhs.isNegative())) ? Sign::Positive :
                                                                Sign::Negative;
        auto [result, bits] = division(lhs.Number(), rhs.Number(), Division::Mode::Div);
        return BigInt(result, bits, sign);
    }

    BigInt& operator /= (const BigInt& other)
    {
        *this = operator/(*this, other);
        return *this;
    }

    friend BigInt operator % (const BigInt& lhs, const BigInt& rhs)
    {
        if(lhs < rhs) return rhs;
        Sign sign;
        auto [result, bits] = division(lhs.Number(), rhs.Number(), Division::Mode::Mod);
        return BigInt(result, bits, sign);
    }

    BigInt& operator %= (const BigInt& other)
    {
        *this = operator%(*this, other);
        return *this;
    }

    friend bool operator < (const BigInt& lhs, const BigInt& rhs)
    {
        return less(lhs.Number(), rhs.Number());
    }

    friend bool operator <= (const BigInt& lhs, const BigInt& rhs)
    {
        return greater(rhs.Number(), lhs.Number());
    }

    friend bool operator > (const BigInt& lhs, const BigInt& rhs)
    {
        return greater(lhs.Number(), rhs.Number());
    }

    friend bool operator >= (const BigInt& lhs, const BigInt& rhs)
    {
        return less(rhs.Number(), lhs.Number());
    }

    friend bool operator == (const BigInt& lhs, const BigInt& rhs)
    {
        return equal(lhs.Number(), rhs.Number());
    }

    friend bool operator != (const BigInt& lhs, const BigInt& rhs)
    {
        return !operator ==(lhs, rhs);
    }

    friend BigInt operator ^ (const BigInt& lhs, const BigInt& rhs)
    {
        return Transform(lhs, rhs, predicates['^']);
    }

    BigInt& operator ^= (const BigInt& other)
    {
        Transform(m_number, other, predicates['^']);
        return *this;
    }

    friend BigInt operator | (const BigInt& lhs, const BigInt& rhs)
    {
        return Transform(lhs, rhs, predicates['|']);
    }

    BigInt& operator |= (const BigInt& other)
    {
        Transform(m_number, other, predicates['|']);
        return *this;
    }

    friend BigInt operator & (const BigInt& lhs, const BigInt& rhs)
    {
        return Transform(lhs, rhs, predicates['&']);
    }

    BigInt& operator &= (const BigInt& other)
    {
        Transform(m_number, other, predicates['&']);
        return *this;
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
            m_sign = Sign::Negative;
            Increment(m_number);
        }
        else
        {

            m_sign = Sign::Positive;
            Decrement(m_number);
        }
        return *this;
    }

    BigInt& operator -()
    {
        m_sign = isPositive() ? Sign::Negative : Sign::Positive;
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
        leftShift(m_number, offset);
        return *this;
    }

    friend BigInt operator >> (const BigInt& lhs, const size_t offset)
    {
        BinaryData newResult = lhs.Number();
        rightShift(newResult, offset);
        return BigInt(newResult, lhs.count(), Sign::Positive); // TODO: sign and count bits
    }

    BigInt& operator >>= (const size_t offset)
    {
        rightShift(m_number, offset);
        return *this;
    }

};

// TODO
namespace Math
{

void abs(BigInt& number)
{
    if(number.isNegative()) number.MakePositive();
}

std::vector<BigInt> factorize()
{}

BigInt fibonacci(std::size_t nElem)
{}

BigInt factorial(std::size_t nElem)
{}

BigInt gcd(const BigInt& a, const BigInt& b)
{
    if(a == b) return a;
    if(a.isZero()) return b;
    if(b.isZero()) return a;

    if(a.isEven())
        return b.isOdd() ? gcd(a>>1, b) : (gcd(a>>1, b>>1) << 1);
}

BigInt lcm(const BigInt& a, const BigInt& b)
{
    return (a*b) / gcd(a,b);
}

} // namespace Math

} // namespace BigInt

