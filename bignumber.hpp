#pragma once
#include <list>
#include <forward_list>
#include <vector>
#include <cmath>
#include <iterator>
#include "Converter.hpp"
#include "Operators.hpp"

namespace BigInt
{

enum class Base
{
    Binary,
    Octal,
    Decimal,
    Hexadecimal,
};

namespace
{

template<typename T>
constexpr bool is_allow_primary()
{
    using Type = std::decay_t<T>;
    return std::is_same_v<Type, int> ||
            std::is_same_v<Type, unsigned> ||
            std::is_same_v<Type, signed> ||
            std::is_same_v<Type, short> ||
            std::is_same_v<Type, unsigned short> ||
            std::is_same_v<Type, signed short> ||
            std::is_same_v<Type, long> ||
            std::is_same_v<Type, signed long> ||
            std::is_same_v<Type, unsigned long> ||
            std::is_same_v<Type, long long> ||
            std::is_same_v<Type, unsigned long long> ||
            std::is_same_v<Type, signed long long> ||
            std::is_same_v<Type, bool> ||
            std::is_same_v<Type, char> ||
            std::is_same_v<Type, uint8_t> ||
            std::is_same_v<Type, uint16_t> ||
            std::is_same_v<Type, uint32_t> ||
            std::is_same_v<Type, uint64_t> ||
            std::is_same_v<Type, int8_t> ||
            std::is_same_v<Type, int16_t> ||
            std::is_same_v<Type, int32_t> ||
            std::is_same_v<Type, int64_t> ||
            std::is_same_v<Type, std::size_t>;
}

template<class Container>
constexpr bool is_allow_container()
{
    using ValueType = typename std::decay_t<Container>::value_type;
    return std::is_same_v<Container, std::string> ||
            (std::is_same_v<Container, std::vector<ValueType>> && is_allow_primary<ValueType>() ) ||
            (std::is_same_v<Container, std::list<ValueType>> && is_allow_primary<ValueType>() ) ||
            (std::is_same_v<Container, std::deque<ValueType>> && is_allow_primary<ValueType>() ) ||
            (std::is_same_v<Container, std::forward_list<ValueType>> && is_allow_primary<ValueType>() ) ||
            (std::is_same_v<Container, std::initializer_list<ValueType>> && is_allow_primary<ValueType>() ) ||
            (std::is_same_v<Container, std::array<ValueType, sizeof (Container)/sizeof(ValueType)>> && is_allow_primary<ValueType>() );
}

std::unordered_map<Base, std::pair<unsigned, unsigned>> limits {
    { Base::Binary, {0, 1} },
    { Base::Octal, {0, 7} },
    { Base::Decimal, {0, 9} },
    { Base::Hexadecimal, {0, 15} },
};

} // namespace

class BigInt final
{
    enum class Sign : bool
    {
        Positive,
        Negative
    };

    template<class T>
    bool validator(T&& value, Base base)
    {
        assert (static_cast<unsigned>(base) < static_cast<unsigned>(Base::Hexadecimal));
        unsigned validatedValue;
        if(std::is_same_v<std::decay_t<T>, char>)
        {
            if(value > '9')
                validatedValue = static_cast<unsigned>(std::tolower(value) - 'a');
            else
                validatedValue = static_cast<unsigned>(std::tolower(value));
        }

        return validatedValue >= limits[base].first &&
                  validatedValue <= limits[base].second;
    }

    template< class TContainer>
    bool isValid(TContainer&& container, Base base)
    {
        using valueType = typename std::decay_t<TContainer>::value_type;
        return std::find_if_not(std::execution::par_unseq, cbegin(container), cend(container),
                     [&](const valueType& value){ return validator(value, base); })
        == container.cend();
    }

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

    const BinaryData& Number() const { return m_number; }

    size_t count() const
    {
        return m_bitSet;
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

};

} // namespace BigInt

