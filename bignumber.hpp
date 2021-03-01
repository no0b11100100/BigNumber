#pragma once
#include <cmath>
#include <iterator>
#include "Converter.hpp"
#include "Operators.hpp"

namespace BigInt
{

class BigInt final
{
    enum class Sign : bool
    {
        Positive,
        Negative
    };
    template<class TransformPredicate, class ForEachPredicate>
    BigInt binaryOperatorsWithAssignment(const BigInt& lhs, const BigInt& rhs,
                           TransformPredicate transformPredicate,
                           ForEachPredicate forEachPredicate)
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

        std::transform(firstStart, firstEnd, secondStart, std::back_inserter(result), transformPredicate);
        std::for_each(secondStart, secondStart, forEachPredicate);

        return BigInt(result, 0, Sign::Positive); // TODO
    }

    template<class TransformPredicate, class ForEachPredicate>
    BigInt& binaryOperatorsWithoutAssignment(BigInt& This, const BigInt& rhs,
                           TransformPredicate transformPredicate,
                           ForEachPredicate forEachPredicate)
    {
        using Iterator = BinaryData::reverse_iterator;
        Iterator end = m_number.size() < rhs.Number().size() ?
                    m_number.rend() :
                    std::next(m_number.rbegin(), m_number.size() - rhs.Number().size());

        std::transform(m_number.rbegin(), end, rhs.Number().rbegin(), transformPredicate);

        Iterator restStart, restEnd;

        if(end == m_number.rend())
        {
            size_t offset = rhs.Number().size() - m_number.size();
            restStart = std::next(std::decay_t<BinaryData>(rhs.Number()).rbegin(), offset);
            restEnd = std::decay_t<BinaryData>(rhs.Number()).rend();
        }
        else
        {
            size_t offset = m_number.size() - rhs.Number().size();
            restStart = std::next(m_number.rbegin(), offset);
            restEnd = m_number.rend();
        }

        std::for_each(restStart, restEnd, forEachPredicate);

        return This;
    }

    BinaryData m_number;
    std::size_t m_bitSet;
    Sign m_sign;

    BigInt(const BinaryData& number, size_t bits, Sign sign):
        m_number{number},
        m_sign{sign},
        m_bitSet{bits}
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

    size_t count() const
    {
        return m_bitSet;
    }

    friend BigInt operator + (const BigInt& lhs, const BigInt& rhs)
    {
        if(lhs.isPositive() && rhs.isPositive())
        {
            auto [result, bits] = addition(lhs.Number(), rhs.Number());
            return BigInt(result, bits, Sign::Positive);
        }
        //  TODO: handle negative and positive cases
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

    friend BigInt operator * (const BigInt& lhs, const BigInt& rhs)
    {
        Sign sign = ( (lhs.isPositive() && rhs.isPositive()) ||
                      (lhs.isNegative() && rhs.isNegative())) ? Sign::Positive :
                                                                Sign::Negative;
        auto[result, bits] = multiplication(lhs.Number(), rhs.Number());
        return BigInt(result, bits, sign);
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

    friend BigInt operator % (const BigInt& lhs, const BigInt& rhs)
    {
        if(lhs < rhs) return rhs;
        Sign sign;
        auto [result, bits] = division(lhs.Number(), rhs.Number(), Division::Mode::Mod);
        return BigInt(result, bits, sign);
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
//        using Iterator = BinaryData::const_reverse_iterator;
//        BinaryData result(std::max(lhs.Number().size(), rhs.Number().size()));
//        Iterator firstStart;
//        Iterator firstEnd;
//        Iterator secondStart;
//        Iterator secondEnd;

//        if(lhs.count() <= rhs.count())
//        {
//            firstStart = lhs.Number().crbegin();
//            firstEnd = lhs.Number().crend();
//            secondStart = rhs.Number().crbegin();
//            secondStart = rhs.Number().crend();
//        }
//        else
//        {
//            firstStart = rhs.Number().crbegin();
//            firstEnd = rhs.Number().crend();
//            secondStart = lhs.Number().crbegin();
//            secondStart = lhs.Number().crend();
//        }

//        std::transform(firstStart, firstEnd, secondStart, std::back_inserter(result), [](const Bit& lhsBit, const Bit& rhsBit)
//        {
//            return lhsBit == rhsBit ? 0 : 1;
//        });

//        std::for_each(secondStart, secondStart, [&result](const Bit& bit)
//        {
//            if(bit == 0) result.push_back(0);
//            else result.push_back(1);
//        });

//        return BigInt(result, Base::Binary);
    }

    BigInt& operator ^= (const BigInt& rhs)
    {
//        using Iterator = BinaryData::reverse_iterator;
//        Iterator end = m_number.size() < rhs.Number().size() ?
//                    m_number.rend() :
//                    std::next(m_number.rbegin(), m_number.size() - rhs.Number().size());

//        std::transform(m_number.rbegin(), end, rhs.Number().rbegin(),
//                       [](const Bit& lhsBit, const Bit& rhsBit)
//        {
//            return lhsBit == rhsBit ? 0 : 1;
//        });

//        Iterator restStart, restEnd;

//        if(end == m_number.rend())
//        {
//            size_t offset = rhs.Number().size() - m_number.size();
//            restStart = std::next(std::decay_t<BinaryData>(rhs.Number()).rbegin(), offset);
//            restEnd = std::decay_t<BinaryData>(rhs.Number()).rend();
//        }
//        else
//        {
//            size_t offset = m_number.size() - rhs.Number().size();
//            restStart = std::next(m_number.rbegin(), offset);
//            restEnd = m_number.rend();
//        }

//        std::for_each(restStart, restEnd, [&](const Bit& bit)
//        {
//            if(bit == 0) m_number.push_back(0);
//            else m_number.push_back(1);
//        });

        return *this;
    }

    bool isPositive() const { return m_sign == Sign::Positive; }
    bool isNegative() const { return isPositive(); }

};

} // namespace BigInt

