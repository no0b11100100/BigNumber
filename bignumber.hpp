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

    template<class T>
    static void Transform(T&& lhs, const BinaryData& rhs, predicateType predicate, BinaryData& result)
    {
        if(lhs.size() <= rhs.size())
            std::transform(lhs.crbegin(), lhs.crend(), rhs.crbegin(), result.rbegin(), predicate);
        else // lhs.size() > rhs.size()
            std::transform(rhs.crbegin(), rhs.crend(), lhs.crbegin(), result.rbegin(), predicate);
    }

    static BigInt binaryOperators(const BinaryData& lhs, const BinaryData& rhs, predicateType predicate)
    {
        BinaryData result(std::max(lhs.size(), rhs.size()));
        Transform(lhs, rhs, predicate, result);
        return BigInt(result, 0, Sign::Positive);
    }

    void binaryOperators(BinaryData& lhs, const BinaryData& rhs, predicateType predicate)
    {
        Transform(lhs, rhs, predicate, lhs);
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
    bool isZero() const { return *m_number.begin() == 0; }

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
        return binaryOperators(lhs.Number(), rhs.Number(), predicates['^']);
    }

    BigInt& operator ^= (const BigInt& other)
    {
        binaryOperators(m_number, other.Number(), predicates['^']);
        return *this;
    }

    friend BigInt operator | (const BigInt& lhs, const BigInt& rhs)
    {
        return binaryOperators(lhs.Number(), rhs.Number(), predicates['|']);
    }

    BigInt& operator |= (const BigInt& other)
    {
        binaryOperators(m_number, other.Number(), predicates['|']);
        return *this;
    }

    friend BigInt operator & (const BigInt& lhs, const BigInt& rhs)
    {
        return binaryOperators(lhs.Number(), rhs.Number(), predicates['&']);
    }

    BigInt& operator &= (const BigInt& other)
    {
        binaryOperators(m_number, other.Number(), predicates['&']);
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

} // namespace BigInt

