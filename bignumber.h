#pragma once
#include <iostream>
#include <list>
#include <vector>
#include <deque>
#include <algorithm>
#include <map>

namespace BigInt
{

namespace
{

template<typename T>
constexpr bool is_integer()
{
    return std::is_same_v<T, int> ||
            std::is_same_v<T, unsigned> ||
            std::is_same_v<T, signed> ||
            std::is_same_v<T, short> ||
            std::is_same_v<T, unsigned short> ||
            std::is_same_v<T, signed short> ||
            std::is_same_v<T, long> ||
            std::is_same_v<T, signed long> ||
            std::is_same_v<T, unsigned long> ||
            std::is_same_v<T, long long> ||
            std::is_same_v<T, unsigned long long> ||
            std::is_same_v<T, signed long long> ||
            std::is_same_v<T, uint8_t> ||
            std::is_same_v<T, uint16_t> ||
            std::is_same_v<T, uint32_t> ||
            std::is_same_v<T, uint64_t> ||
            std::is_same_v<T, std::size_t>;
}

template<typename T>
constexpr bool is_allow_container()
{
    return std::is_same_v<T, std::vector> ||
           std::is_same_v<T, std::list> ||
           std::is_same_v<T, std::deque>;
}

template<class T>
using is_integer_t = typename std::enable_if_t< std::is_same_v< is_integer<T>(), true > >;

template <class Container>
using is_container_t = typename std::enable_if_t< std::is_same_v< is_allow_container<Container>(), true > &&
                                                  std::is_same_v< is_integer_t<typename Container::value_type>(), true > >;

} // namespace

enum class BASE
{
    BINARY,
    OCTAL,
    DECIMAL,
    HEXADECIMAL
};

class BigInt
{
    std::list<bool> m_number;
    void toBinary(int number)
    {
        while(number > 0)
        {
            m_number.push_front(static_cast<bool>(number % 2));
            number /= 2;
        }
    }

    template<typename T>
    void shift(std::list<T>& number, int loops, bool direction = false)
    {
        if(!direction) for(int i = 0; i < loops; ++i) number.pop_back();
        else for(int i = 0; i < loops; ++i) number.push_back(0);
    }

public:
    template< typename T, is_integer_t<T> >
    BigInt(T){}

    template < class T, is_container_t<T> >
    BigInt(T){}

    BigInt() {}
    BigInt(int number)
    {
        toBinary(number);
        for(auto v : m_number)
        {
            std::cout << v << " ";
        } std::cout << std::endl;
    }

    BigInt operator / (int number)
    {
        auto newNumber = m_number;
        shift(newNumber, number / 2);
        for(auto v : newNumber)
        {
            std::cout << v << " ";
        } std::cout << std::endl;

        if(number % 2 != 0)
        {
//            newNumber -= m_number;
        }
        return BigInt();
    }

    BigInt operator * (int number)
    {
        auto newNumber = m_number;
        shift(newNumber, number / 2, true);
        for(auto v : newNumber)
        {
            std::cout << v << " ";
        } std::cout << std::endl;

        if(number % 2 != 0)
        {
//            newNumber += m_number;
        }

        return BigInt();
    }

    BigInt operator - (int number)
    {
        return BigInt();
    }

    BigInt operator + (int number)
    {
        return BigInt();
    }
};

} // namespace BigInt
