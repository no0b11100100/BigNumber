#pragma once
#include <iostream>
#include <list>
#include <vector>
#include <deque>
#include <string>
#include <algorithm>
#include <map>
#include <cassert>

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

    std::list<bool> toBinary(int number, int)
    {
        std::list<bool> binary;
        while(number > 0)
        {
            binary.push_front(static_cast<bool>(number % 2));
            number /= 2;
        }

        return binary;
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

    BigInt(std::list<bool> number): m_number{std::move(number)}
    {}

    BigInt(int number)
    {
        toBinary(number);
    }

    void Print() const
    {
        for(const auto& bit : m_number)
        {
            std::cout << bit << " ";
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

        return BigInt();
    }

    BigInt operator * (int number) // https://ideone.com/IfJVQX
    {
        BigInt result(0);
        auto newNumber = m_number;
        auto str = toBinary(number, 1);

        for(int i = 0; str.size() != 0; ++i)
        {
            auto bit = std::next(str.begin(), i);
            if(*(str.rbegin()) == 1) result = result + newNumber;
            shift(newNumber, 1, true);
            shift(str, 1);
        }
        return result;
    }

    BigInt operator - (int number)
    {
        return BigInt();
    }

    BigInt operator + (int number)
    {
        return this->operator+(toBinary(number, 1));
    }

    BigInt operator + (std::list<bool> number)
    {
        if(m_number.size() < number.size())
        {
            int diff = number.size() - m_number.size();
            for(int i = 0; i < diff; ++i) m_number.push_front(0);
        }
        else
        {
            int diff = m_number.size() - number.size();
            for(int i = 0; i < diff; ++i) number.push_front(0);
        }

        assert(m_number.size() == number.size());

        int t = 0;
        int size = m_number.size();
        std::list<bool> result( size + 1 );
        auto index = [](std::list<bool>::iterator it, int index) { return (std::next(it, index)); };

        for(int i = size - 1; i > -1; i--)
        {
            int n = *(index(begin(m_number), i)) + *(index(begin(number), i)) + t;
            t = n / 2;
            *(index(begin(result), i+1)) = n % 2;
        }

        *(index(begin(result), 0)) = t;
        return BigInt(result);
    }
};

} // namespace BigInt
