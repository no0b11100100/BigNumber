#pragma once
#include <iostream>
#include <list>
#include <forward_list>
#include <vector>
#include <deque>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <cassert>
#include <cmath>
#include <array>
#include <algorithm>
#include <initializer_list>
#include <execution>
#include "Operators.h"

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

} // namespace

namespace BigInt
{

enum class Base
{
    Binary,
    Octal,
    Decimal,
    Hexadecimal,
    Unknown
};

class BigInt final
{
    enum class Sign : bool
    {
        Positive,
        Negative
    };
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
};

} // namespace BigInt

