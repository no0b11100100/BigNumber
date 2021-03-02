#pragma once
#include <cmath>
#include <iterator>
#include "Converter.hpp"
#include "Operators.hpp"

namespace BigInt
{

namespace
{

using predicatesType = std::tuple<std::function<Bit(const Bit&, const Bit&)>,
                                std::function<Bit(const Bit&)>>;

std::unordered_map<char, predicatesType> predicates
{
    { '^', std::make_tuple([](const Bit& lhsBit, const Bit& rhsBit) { return lhsBit == rhsBit ? 0 : 1; },
                           [](const Bit& bit) { return bit == 0 ? 0 : 1; }) },

    {'|', { std::make_tuple([](const Bit& lhsBit, const Bit& rhsBit) { return lhsBit == rhsBit ? lhsBit : 1; },
            [](const Bit& bit) { return bit == 0 ? 0 : 1; }) } },

    {'&', { std::make_tuple([](const Bit& lhsBit, const Bit& rhsBit) { return lhsBit == rhsBit ? lhsBit : 0; },
            [](const Bit&) { return 0; }) } },
};

}// namespace

class BigInt final
{
    enum class Sign : bool
    {
        Positive,
        Negative
    };

    constexpr static const unsigned TransfromPredicate = 0;
    constexpr static const unsigned ForEachPredicate = 1;

    static BigInt binaryOperatorsWithoutAssignment(const BigInt& lhs, const BigInt& rhs, predicatesType predicates)
    {
        using Iterator = BinaryData::const_reverse_iterator;
        BinaryData result(std::max(lhs.Number().size(), rhs.Number().size()));
        Iterator firstStart;
        Iterator firstEnd;
        Iterator secondStart;
        Iterator secondEnd;

        if(lhs.count() <= rhs.count())
        {
            firstStart = lhs.Number().crbegin();
            firstEnd = lhs.Number().crend();
            secondStart = rhs.Number().crbegin();
            secondStart = rhs.Number().crend();
        }
        else
        {
            firstStart = rhs.Number().crbegin();
            firstEnd = rhs.Number().crend();
            secondStart = lhs.Number().crbegin();
            secondStart = lhs.Number().crend();
        }

        std::transform(firstStart, firstEnd, secondStart, std::back_inserter(result), std::get<TransfromPredicate>(predicates));
        std::for_each(secondStart, secondStart, [&](const Bit& bit)
        {
            Bit newBit = std::get<ForEachPredicate>(predicates)(bit);
            result.push_back(newBit);
        });

        return BigInt(result, 0, Sign::Positive); // TODO
    }

    void binaryOperatorsWithAssignment(BinaryData& This, const BigInt& rhs, predicatesType predicates)
    {
        using Iterator = BinaryData::reverse_iterator;
        Iterator end = m_number.size() < rhs.Number().size() ?
                    m_number.rend() :
                    std::next(m_number.rbegin(), m_number.size() - rhs.Number().size());

        std::transform(This.rbegin(), end, rhs.Number().rbegin(), std::get<TransfromPredicate>(predicates));

        Iterator restStart, restEnd;

        if(end == This.rend())
        {
            size_t offset = rhs.Number().size() - m_number.size();
            restStart = std::next(std::decay_t<BinaryData>(rhs.Number()).rbegin(), offset);
            restEnd = std::decay_t<BinaryData>(rhs.Number()).rend();
        }
        else
        {
            size_t offset = This.size() - rhs.Number().size();
            restStart = std::next(This.rbegin(), offset);
            restEnd = This.rend();
        }

        std::for_each(restStart, restEnd, [&](const Bit& bit)
        {
            Bit newBit = std::get<ForEachPredicate>(predicates)(bit);
            This.push_back(newBit);
        });
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
        *this = operator-(*this, other);
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
        return binaryOperatorsWithoutAssignment(lhs, rhs, predicates['^']);
    }

    BigInt& operator ^= (const BigInt& other)
    {
        binaryOperatorsWithAssignment(m_number, other, predicates['^']);
        return *this;
    }

    friend BigInt operator | (const BigInt& lhs, const BigInt& rhs)
    {
        return binaryOperatorsWithoutAssignment(lhs, rhs, predicates['|']);
    }

    BigInt& operator |= (const BigInt& other)
    {
        binaryOperatorsWithAssignment(m_number, other, predicates['|']);
        return *this;
    }

    friend BigInt operator & (const BigInt& lhs, const BigInt& rhs)
    {
        return binaryOperatorsWithoutAssignment(lhs, rhs, predicates['&']);
    }

    BigInt& operator &= (const BigInt& other)
    {
        binaryOperatorsWithAssignment(m_number, other, predicates['&']);
        return *this;
    }

    friend BigInt operator << (const BigInt& lhs, const size_t offset)
    {
        BinaryData newResult = lhs.Number();
        leftShift(newResult, offset);
    }

    BigInt operator <<= (const size_t offset)
    {
        leftShift(m_number, offset);
    }

    friend BigInt operator >> (const BigInt& lhs, const size_t offset)
    {
        BinaryData newResult = lhs.Number();
        rightShift(newResult, offset);
    }

    BigInt operator >>= (const size_t offset)
    {
        rightShift(m_number, offset);
    }

};

} // namespace BigInt

