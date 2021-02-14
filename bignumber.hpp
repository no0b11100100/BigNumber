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
#include <climits>
#include <unordered_map>
#include <type_traits>
#include <array>

namespace
{

template <typename T>
struct identity
{
    using type = T;
};

template<typename T>
struct remove_all_pointers : std::conditional_t<
    std::is_pointer_v<T>,
    remove_all_pointers<
        std::remove_pointer_t<T>
    >,
    identity<T>
>
{};

template<typename T>
using remove_all_t = typename std::decay_t<typename remove_all_pointers<T>::type>;


template<typename T>
constexpr bool is_allow_primary()
{
    using Type = remove_all_t<T>;
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
            std::is_same_v<Type, uint8_t> ||
            std::is_same_v<Type, uint16_t> ||
            std::is_same_v<Type, uint32_t> ||
            std::is_same_v<Type, uint64_t> ||
            std::is_same_v<Type, int8_t> ||
            std::is_same_v<Type, int16_t> ||
            std::is_same_v<Type, int32_t> ||
            std::is_same_v<Type, int64_t> ||
            std::is_same_v<Type, std::size_t> ||
            std::is_same_v<Type, bool>;
}

template<class Container>
constexpr bool is_allow_container()
{
    using ValueType = typename remove_all_t<Container>::value_type;
    return std::is_same_v<Container, std::string> ||
            (std::is_same_v<Container, std::vector<ValueType>> && is_allow_primary<ValueType>() ) ||
            (std::is_same_v<Container, std::list<ValueType>> && is_allow_primary<ValueType>() ) ||
            (std::is_same_v<Container, std::deque<ValueType>> && is_allow_primary<ValueType>() ) ||
            (std::is_same_v<Container, std::forward_list<ValueType>> && is_allow_primary<ValueType>() ) ||
            (std::is_same_v<Container, std::array<ValueType, sizeof (Container)/sizeof(ValueType)>> && is_allow_primary<ValueType>() );
}

} // namespace


namespace BigInt
{

enum class BASE
{
    BINARY,
    OCTAL,
    DECIMAL,
    HEXADECIMAL
};

enum class SIGN : bool
{
    POSITIVE,
    NEGATIVE
};

class BigInt
{
    std::list<bool> m_number;
    SIGN m_sign;
    std::size_t m_bitSet;

public:

    template<typename Type, class = typename std::enable_if_t< is_allow_primary<Type>() > >
    BigInt(Type&){}

    template<typename Type, class = typename std::enable_if_t< is_allow_primary<Type>() > >
    BigInt(const Type&){}

    template<typename Type, class = typename std::enable_if_t< is_allow_primary<Type>() > >
    BigInt(Type&&){}

    template<typename Type, class = typename std::enable_if_t<is_allow_container<Type>()>>
    BigInt(Type&, BASE base){}

    template<typename Type, class = typename std::enable_if_t<is_allow_container<Type>()>>
    BigInt(const Type&, BASE base){}

    template<typename Type, class = typename std::enable_if_t<is_allow_container<Type>()>>
    BigInt(Type&&, BASE base){}

    BigInt() :
        m_number{0},
        m_sign{SIGN::POSITIVE},
        m_bitSet{0}
    {}
};

} // namespace BigInt
