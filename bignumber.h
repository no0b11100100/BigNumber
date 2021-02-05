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
using is_container_t = typename std::enable_if_t< std::is_same_v<Container, std::string> || (std::is_same_v< is_allow_container<Container>(), true > &&
                                                  std::is_same_v< is_integer_t<typename Container::value_type>(), true >) >;

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
    std::size_t setedBits;
    void toBinary(int number)
    {
        while(number > 0)
        {
            m_number.push_front(static_cast<bool>(number % 2));
            number >>= 1;
        }
    }

    std::list<bool> toBinary(int number, int)
    {
        std::list<bool> binary;
        while(number > 0)
        {
            binary.push_front(static_cast<bool>(number % 2));
            number >>= 1;
        }

        return binary;
    }

    template<typename T>
    void shift(std::list<T>& number, std::size_t loops, bool direction = false)
    {
        if(!direction)
        {
            if (number.size() < loops)
            {
                number.clear();
                number.push_back(0);
                return;
            }
            for(std::size_t i = 0; i < loops; ++i)
            {
                number.pop_back();
            }
        }
        else for(std::size_t i = 0; i < loops; ++i) number.push_back(0);
    }

    void removeZeros(std::list<bool>& number)
    {
        for(auto it = begin(number); it != end(number);)
        {
            if(*it == 0) it = number.erase(it);
            else
            {
                ++it;
                break;
            }
        }
    }

    std::size_t countSetedBits() const
    {
        return std::count(begin(m_number), end(m_number), 1);
    }

    std::size_t toLow2Pow(std::size_t number)
    {
        --number;
        number |= number >> 1;
        number |= number >> 2;
        number |= number >> 4;
        number |= number >> 8;
        number |= number >> 16;
//        number |= number >> 32;
        number >>= 1;
        return ++number;
    }

public:
    template< typename T, is_integer_t<T> >
    BigInt(T){}

    template < class T, is_container_t<T> >
    BigInt(T){}

    BigInt() {}

    BigInt(std::list<bool> number):
        m_number{std::move(number)},
        setedBits{countSetedBits()}
    {}

    BigInt(int number)
    {
        toBinary(number);
        setedBits = countSetedBits();
    }

    std::size_t Decimal() const
    {
        std::size_t result = 0;
        int degree = 0;
        for(auto bit = m_number.rbegin(); bit != m_number.rend(); ++bit)
            result += static_cast<int>(*bit) << degree++;

        return result;
    }

    void Print() const
    {
        for(const auto& bit : m_number)
        {
            std::cout << bit << " ";
        } std::cout << std::endl;
    }

    BigInt toLow2Pow()
    {
        std::list<bool> result;
        result.push_back(1);
        for(std::size_t i = 1; i < m_number.size(); ++i) result.push_back(0);

        return BigInt(result);
    }

    bool is2Pow() const
    {
        if(m_number.size() == 1 && *(m_number.begin()) == 1) return true;
        std::list<bool> result = m_number;
        result.pop_front();
        for(auto bit : result) if (bit != 0) return false;
        return true;
    }

    std::size_t SetedBits() const
    {
        return setedBits;
    }

    std::list<bool> List() const
    {
        return m_number;
    }

    BigInt operator / (int number)
    {
        return this->operator/(toBinary(number, 1));
    }

    BigInt operator / (std::list<bool> number) // https://stackoverflow.com/questions/20637339/binary-long-division-algorithm
    {
        BigInt result;
        BigInt Number = *this;
        while(!m_number.empty())
        {
            if(*(number.rbegin()) == 0 && *(m_number.rbegin()) == 0)
            {
                shift(number, 1);
                shift(m_number, 1);
                result = result + 2;
            } else
            {
                result = result + 1;
                *this = *this - number;
            }
        }

        *this = std::move(Number);

        return result;
    }

    BigInt operator / (BigInt divisor)
    {
        BigInt result(0);
        BigInt dividend = *this;
        while(dividend >= divisor)
        {
            int diff = (dividend.List().size() - divisor.List().size());
            auto temp = divisor << diff;

            if(temp > dividend)
            {
                temp >>= 1;
                --diff;
            }

            dividend -= temp;
            result += BigInt(1)<<diff;
        }

        return result;
    }

    BigInt operator * (int number)
    {
        return this->operator*(toBinary(number, 1));
    }

    BigInt operator * (std::list<bool> number) // https://ideone.com/IfJVQX
    {
        BigInt result(0);
        auto newNumber = m_number;

        while(!number.empty())
        {
            if(*(number.rbegin()) == 1) result = result + newNumber;
            shift(newNumber, 1, true);
            shift(number, 1);
        }
        return result;
    }

    BigInt operator * (BigInt number)
    {
        return operator*(number.List());
    }

    BigInt operator - (int number)
    {
        return this->operator-(toBinary(number, 1));
    }

    BigInt operator - (std::list<bool> number)
    {
        if(m_number.size() < number.size()) return BigInt();
        std::string num_1 = "";
        std::string num_2 = "";

        for(const auto& bit : m_number) num_1 += bit + '0';
        for(const auto& bit : number) num_2 += bit + '0';

        auto res = toBinary( (std::strtol(num_1.c_str(), NULL, 2) - std::strtol(num_2.c_str(), NULL, 2)), 1);
        return BigInt(res);
    }

    BigInt operator - (BigInt number)
    {
        return this->operator-(number.List());
    }

    BigInt& operator -=(BigInt number)
    {
        *this = operator-(number.List());
        return *this;
    }

    BigInt operator + (int number)
    {
        return this->operator+(toBinary(number, 1));
    }

    BigInt operator + (BigInt number)
    {
        return this->operator+(number.List());
    }

    BigInt operator + (std::list<bool> number)
    {
//        if(m_number.size() < number.size())
//        {
//            int diff = number.size() - m_number.size();
//            for(int i = 0; i < diff; ++i) m_number.push_front(0);
//        }
//        else
//        {
//            int diff = m_number.size() - number.size();
//            for(int i = 0; i < diff; ++i) number.push_front(0);
//        }

//        assert(m_number.size() == number.size());

//        int t = 0;
//        int size = m_number.size();
//        std::list<bool> result( size + 1 );
//        auto index = [](std::list<bool>::iterator it, int index) { return std::next(it, index); };

//        for(int i = size - 1; i >= 0; --i)
//        {
//            int n = *(index(begin(m_number), i)) + *(index(begin(number), i)) + t;
//            t = n << 1;
//            *(index(begin(result), i+1)) = n % 2;
//        }

//        *(index(begin(result), 0)) = t;
//        return BigInt(result);

        std::string num_1 = "";
        std::string num_2 = "";

        for(const auto& bit : m_number) num_1 += bit + '0';
        for(const auto& bit : number) num_2 += bit + '0';

        return BigInt(std::strtol(num_1.c_str(), NULL, 2) + std::strtol(num_2.c_str(), NULL, 2));
    }

    BigInt& operator += (BigInt other)
    {
        *this = operator+(other);
        return *this;
    }

    BigInt operator ~()
    {
        auto newNumber = m_number;
        for(auto& bit : newNumber)
            bit = bit == 1 ? 0 : 1;

        return BigInt(newNumber);
    }

    BigInt operator |(BigInt other)
    {
        auto number = other.List();
        if(m_number.size() < number.size())
        {
            int diff = other.List().size() - m_number.size();
            for(int i = 0; i < diff; ++i) m_number.push_front(0);
        }
        else
        {
            int diff = m_number.size() - number.size();
            for(int i = 0; i < diff; ++i) number.push_front(0);
        }

        std::list<bool> newNumber(m_number.size());

        for(auto it = m_number.begin(), it_1 = number.begin(); it != m_number.end(); ++it, ++it_1)
        {
            if(*it == *it_1) newNumber.push_back(*it);
            else newNumber.push_back(1);
        }

        removeZeros(newNumber);

        return BigInt(newNumber);
    }

    BigInt operator &(BigInt other)
    {
        auto number = other.List();
        if(m_number.size() < number.size())
        {
            int diff = other.List().size() - m_number.size();
            for(int i = 0; i < diff; ++i) m_number.push_front(0);
        }
        else
        {
            int diff = m_number.size() - number.size();
            for(int i = 0; i < diff; ++i) number.push_front(0);
        }

        std::list<bool> newNumber(m_number.size());

        for(auto it = m_number.begin(), it_1 = number.begin(); it != m_number.end(); ++it, ++it_1)
        {
            if(*it == *it_1) newNumber.push_back(*it);
            else newNumber.push_back(0);
        }

        removeZeros(newNumber);

        return BigInt(newNumber);
    }

    BigInt operator |=(BigInt other)
    {
        *this = operator|(other);
        return *this;
    }

    BigInt operator &=(BigInt other)
    {
        *this = operator&(other);
        return *this;
    }

    BigInt operator ^(BigInt other)
    {
        auto number = other.List();
        if(m_number.size() < number.size())
        {
            int diff = other.List().size() - m_number.size();
            for(int i = 0; i < diff; ++i) m_number.push_front(0);
        }
        else
        {
            int diff = m_number.size() - number.size();
            for(int i = 0; i < diff; ++i) number.push_front(0);
        }

        std::list<bool> newNumber(m_number.size());

        for(auto it = m_number.begin(), it_1 = number.begin(); it != m_number.end(); ++it, ++it_1)
        {
            if(*it == *it_1) newNumber.push_back(0);
            else newNumber.push_back(1);
        }

        removeZeros(newNumber);

        return BigInt(newNumber);
    }

    BigInt operator ^=(BigInt other)
    {
        *this = operator^(other);
        return *this;
    }

     BigInt operator !()
     {
         if(m_number.size() == 1 && *(m_number.begin() ) == 0) return BigInt(1);
         return BigInt(1);
     }

    BigInt operator << (int shifts)
    {
        auto newNumber = m_number;
        shift(newNumber, shifts, true);
        return BigInt(newNumber);
    }

    BigInt& operator <<= (int shifts)
    {
        *this = operator <<(shifts);
        return *this;
    }

    BigInt operator >> (int shifts)
    {
        auto newNumber = m_number;
        shift(newNumber, shifts);
        return BigInt(newNumber);
    }

    BigInt& operator >>= (int shifts)
    {
        *this = operator >>(shifts);
        return *this;
    }

    bool operator > (BigInt other)
    {
        if(m_number.size() > other.List().size()) return true;
        if(m_number.size() < other.List().size()) return false;

        assert(m_number.size() == other.List().size());

        auto value = other.List();

        for(auto it = m_number.begin(), it_1 = value.begin(); it != m_number.end(); ++it, ++it_1)
        {
            if(*it != *it_1) return *it < *it_1 ? false : true;
        }

        return false;
    }

    bool operator < (BigInt other)
    {
        if(m_number.size() < other.List().size()) return true;
        if(m_number.size() > other.List().size()) return false;

        assert(m_number.size() == other.List().size());

        auto value = other.List();

        for(auto it = m_number.begin(), it_1 = value.begin(); it != m_number.end(); ++it, ++it_1)
        {
            if(*it != *it_1) return *it > *it_1 ? false : true;
        }

        return false;
    }

    bool operator >= (BigInt other)
    {
        return !(operator<(other));
    }

    bool operator <= (BigInt other)
    {
        return !(operator>(other));
    }

    bool operator == (BigInt other)
    {
        if(m_number.size() != other.List().size()) return false;

        assert(m_number.size() == other.List().size());

        auto value = other.List();

        for(auto it = m_number.begin(), it_1 = value.begin(); it != m_number.end(); ++it, ++it_1)
        {
            if(*it != *it_1) return false;
        }

        return true;
    }

    bool operator != (BigInt other)
    {
        return !(operator==(other));
    }

    bool isEven() const
    {
        return !m_number.empty() && *(std::next(m_number.rbegin(), 1)) == 0;
    }

    bool isOdd() const
    {
        return !isEven();
    }

    void PushBack(bool bit)
    {
        m_number.push_back(bit);
    }

    void PushFront(bool bit)
    {
        m_number.push_front(bit);
    }
};

} // namespace BigInt
