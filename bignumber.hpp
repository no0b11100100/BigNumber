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
#include <cstdlib>
#include <initializer_list>
#include <execution>

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
            std::is_same_v<Type, bool> ||
            std::is_same_v<Type, char>;
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

// TODO: for all operators: take ranges
struct Subtraction
{
    std::deque<bool> operator()(const BigInt& rhd, const BigInt& lhd)
    {
        std::deque<bool> result;
        std::size_t size = std::max(rhd.count(), lhd.count());
        bool isLoan = false;
        auto rhd_it = crbegin(rhd.List());
        auto rhd_end_it = crend(rhd.List());
        auto lhd_it = crbegin(lhd.List());
        auto lhd_end_it = crend(lhd.List());
        auto addRestElement = [&isLoan](std::deque<bool>& number,
                                            std::deque<bool>::const_reverse_iterator restElementsIt,
                                            std::deque<bool>::const_reverse_iterator endIt)
        {
            if(isLoan)
            {
                for(; restElementsIt != endIt; ++restElementsIt)
                {
                    if(*restElementsIt == 1)
                    {
                        number.push_front(0);
                        ++restElementsIt;
                        isLoan = false;
                        break;
                    }

                    number.push_front(1);
                }
            }

            for(;restElementsIt != endIt; ++restElementsIt)
                number.push_front(*restElementsIt);

            assert(restElementsIt == endIt);
        };

        for(size_t i = size; i >= 0; --i, ++rhd_it, ++lhd_it)
        {
            if(lhd_it == lhd_end_it)
            {
                addRestElement(result, rhd_it, rhd_end_it);
                break;
            }

            bool bit = *rhd_it;

            if(bit > *lhd_it) // 1 0
            {
                if(isLoan) result.push_front(0);
                else
                {
                    result.push_front(1);
                    isLoan = false;
                }

            }
            else if(bit == 1 && bit == *lhd_it) // 1 1
            {
                isLoan = false;
                result.push_front(0);
            }
            else if( bit == *lhd_it) // 0 0
            {
                if(isLoan) result.push_front(1);
                else result.push_front(0);
            }
            else // 0 1
            {
                if(!isLoan)
                {
                    result.push_front(1);
                    isLoan = true;
                }
                else
                    result.push_front(0);
            }
        }

        return result;
    }
} static subtraction;

struct Addition
{
    std::deque<bool> operator()(const BigInt& rhd, const BigInt& lhd)
    {
        std::deque<bool> result;
        std::size_t size = std::max(rhd.count(), lhd.count());
        bool isTransfer = false;
        auto rhd_it = crbegin(rhd.List());
        auto rhd_end_it = crend(rhd.List());
        auto lhd_it = crbegin(lhd.List());
        auto lhd_end_it = crend(lhd.List());

        auto addRestElement = [&isTransfer](std::deque<bool>& number,
                                            std::deque<bool>::const_reverse_iterator restElementsIt,
                                            std::deque<bool>::const_reverse_iterator endIt)
        {
            if(isTransfer)
            {
                for(; restElementsIt != endIt; ++restElementsIt)
                {
                    if(*restElementsIt == 0)
                    {
                        number.push_front(1);
                        isTransfer = false;
                        ++restElementsIt;
                        break;
                    }

                    number.push_front(0);
                }
            }

            for(auto it = restElementsIt; it != endIt; ++it)
                number.push_front(*it);

            if(isTransfer) number.push_front(1);
        };

        for(size_t i = size; i >= 0; --i, ++rhd_it, ++lhd_it)
        {
            if(rhd_it == rhd_end_it)
            {
                addRestElement(result, lhd_it, lhd_end_it);
                break;
            }
            else if (lhd_it == lhd_end_it)
            {
                addRestElement(result, rhd_it, rhd_end_it);
                break;
            }

            if(*rhd_it == 1 && *lhd_it == 1)
            {
                if(isTransfer)
                    result.push_front(1);
                else
                {
                    isTransfer = true;
                    result.push_front(0);
                }
            } else if(*rhd_it == 0 && *lhd_it == 0)
            {
                isTransfer ? result.push_front(1) : result.push_front(0);
                isTransfer = false;
            } else if(*rhd_it != *lhd_it)
                isTransfer ? result.push_front(0) : result.push_front(1);
        }

        return result;
    }
} static addition;

struct Division
{
    /*
    if(compare) add 1
    if(compare: dividend == divisor) find first dividend bit which is greater then 0 and before fill 0 after 1
    else find first dividend bit which is greater then 0 and before fill 0 after 1

    count 1 example:
    1111 -> 1000 + 3 -> count all 1(n) and it is pow(2, n) + n-1

    */
    std::deque<bool> operator()(const BigInt& dividend, const BigInt& divisor, bool isMode = false)
    {
        BigInt tmp;
        BigInt result;
        BigInt unit(1);
        BigInt dividend_ = dividend;
        std::deque<bool> a;
        while(dividend_ >= divisor)
        {
            size_t diff = dividend_.Bit() - divisor.Bit();

            // TODO: test with offset with 1 -> ok for operator %
            if( compare(dividend_.List().cbegin(), divisor.List().cbegin(), divisor.List().cend()) )
            {
                tmp = divisor << diff;
            }
            else
            {
                --diff;
                tmp = divisor << diff;
            }

            if(!isMode)
                dividend_ = minus(dividend_.List(), tmp.List());
            result = plus(result.List(), (unit<<diff).List()); // optimase unit<<diff ??
        }

        for(auto v : result.List()) std::cout << v << std::endl;

        return a;
    }
    /*
        1011111
        101

        1011111
        1010000 -> 16

        1111
        1010
        101 -> 2

        110
        101 -> 1
        000

        res = 19

        #######
        1100001
        1011111



    */

private:
    bool compare(std::deque<bool>::const_iterator it, std::deque<bool>::const_iterator start, std::deque<bool>::const_iterator end)
    {
        for(; start != end; ++it, ++start, ++end)
            if(*it != *start && *it > *start)
                return true;

        return false;
    }

//    BigInt offsetByUnits(std::deque<bool>& number, size_t offset)
//    {
//        for(size_t i {0}; i < offset; ++i)
//            number.push_back(1);
//        return BigInt(number, SIGN::POSITIVE);
//    }

    BigInt minus(std::deque<bool> a, std::deque<bool> b){
        std::string s1;
        std::string s2;

        for(auto v : a) s1 += std::to_string(v);
        for(auto v : b) s2 += std::to_string(v);

        return BigInt( std::strtoull(s1.c_str(), NULL, 2) - std::strtoull(s2.c_str(), NULL, 2)  );
    };

    BigInt plus(std::deque<bool> a, std::deque<bool> b){
        std::string s1;
        std::string s2;

        for(auto v : a) s1 += std::to_string(v);
        for(auto v : b) s2 += std::to_string(v);

        return BigInt( std::strtoull(s1.c_str(), NULL, 2) + std::strtoull(s2.c_str(), NULL, 2)  );
    };
} static division;

struct Multiplication
{
    std::deque<bool> operator()(const BigInt& rhs, const BigInt& lhs)
    {
        BigInt res;
        BigInt tmp = lhs;
        size_t offset = 0;
        std::deque<bool> a;
        auto handleZero = [&]()
        {
            if(offset >= 1)
            {
                std::cout << "distance " << offset << std::endl;
//                res = plus(res.List(), tmp.List());
                res = res - tmp;
                if(offset > 1) res = res - lhs;//res = minus(res.List(), lhs.List());
                offset = 0;
            }
        };

        for(auto it = crbegin(rhs.List()); it != crend(rhs.List()); ++it)
        {
            if(*it == 0) tmp <<= 1;
            if(*it == 1)
            {
                ++offset;
                if( *std::next(it, 1) == 0 || std::next(it, 1) == crend(rhs.List()) ) handleZero();
                tmp <<= 1;
            }
        }

        return a;
    }
private:
//    BigInt minus(std::deque<bool> a, std::deque<bool> b){
//        std::string s1;
//        std::string s2;

//        for(auto v : a) s1 += std::to_string(v);
//        for(auto v : b) s2 += std::to_string(v);

//        return BigInt( std::strtoull(s1.c_str(), NULL, 2) - std::strtoull(s2.c_str(), NULL, 2)  );
//    };

//    BigInt plus(std::deque<bool> a, std::deque<bool> b){
//        std::string s1;
//        std::string s2;

//        for(auto v : a) s1 += std::to_string(v);
//        for(auto v : b) s2 += std::to_string(v);

//        return BigInt( std::strtoull(s1.c_str(), NULL, 2) + std::strtoull(s2.c_str(), NULL, 2)  );
//    };
} static multiplication;

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

class BigInt final
{
    std::deque<bool> m_number;
    SIGN m_sign;
    std::size_t m_bitSet;

    template<class T>
    std::deque<bool> toBinary(T&& number)
    {
        std::deque<bool> list_number;
        if(number == 0){
            list_number.push_back(0);
            m_sign = SIGN::POSITIVE;
            m_bitSet = 0;
            return list_number;
        }

        if(number < 0) {
            number = -number;
            m_sign = SIGN::NEGATIVE;
        }
        else m_sign = SIGN::POSITIVE;

        while(number > 0)
        {
            bool value = static_cast<bool>(number % 2);
            if(value == 1) ++m_bitSet;
            list_number.push_front(value);
            number >>= 1;
        }

        return list_number;
    }

    template<class T>
    std::deque<bool> toBinary(T&& number, BASE base)
    {
        if(!number.empty())
        {
            if( *(number.begin()) > 0) m_sign = SIGN::POSITIVE;
            else m_sign = SIGN::NEGATIVE;
        }

        std::deque<bool> list_number;

        // TODO: handle initializer_list
        if(number.empty())
        {
            list_number.push_back(0);
            m_sign = SIGN::POSITIVE;
            m_bitSet = 0;
            return list_number;
        }

        switch (base) {
        case BASE::BINARY:
            assert(number.size() > 1);

            for(auto it = std::next(begin(number), 1); it != end(number); ++it)
            {
                if(*it == 1) ++m_bitSet;
                list_number.push_back(*it);
            }
            break;
        case BASE::OCTAL:
            break;
        case BASE::DECIMAL:
            // TODO: impl divmod num on 2 and add
            // https://godbolt.org/z/xvYcG6
//            auto divMod = [](T&& number)
//            {
//                int rem = 0;
//                int dvnd;
//                int quot;
//                for(auto it = begin(number); it != end(number); ++it){
//                    dvnd = (rem * 10) + *it;
//                    rem = dvnd % 2; // optimaze dvnd & 1
//                    quot = dvnd / 2;
//                    *it = quot;
//                }

//                return static_cast<bool>(rem);
//            };

//            std::deque<bool> num;
//            while(!number.empty())
//                num.push_back( divMod(number) );

            break;
        case BASE::HEXADECIMAL:
            break;
        default:
            break;
        }

        return list_number;
    }

    void removeZeros()
    {
        while(true)
        {
            auto it = begin(m_number);
            if(m_number.size() == 1 && *it == 0) break;
            if(*it == 0) m_number.pop_back();
            else break;
        }
    }

    template<class T>
    bool verification(T&& number, BASE base)
    {
        using constValueTypeReference =
        std::conditional_t<
            std::is_same_v<remove_all_t<T>, std::string>,
            const char&,
            const typename T::value_type&
        >;

        typename T::iterator validate;
        switch(base) // TODO: avoid magic numbers
        {
        case BASE::BINARY:
            validate = std::find_if(std::execution::par_unseq, number.begin(), number.end(), [](constValueTypeReference value)
            { return value != 0 || value != 1 || value != '0' || value != '1';});
            return validate == number.end() ? true : false;

        case BASE::OCTAL:
            validate = std::find_if(std::execution::par_unseq, number.begin(), number.end(), [](constValueTypeReference value)
            { return value < 0 || value > 7 || value < '0' || value > '7'; });
            return validate == number.end() ? true : false;

        case BASE::DECIMAL:
            validate = std::find_if(std::execution::par_unseq, number.begin(), number.end(), [](constValueTypeReference value)
            { return value < 0 || value > 10 || value < '0' || value > '9'; });
            return validate == number.end() ? true : false;

        case BASE::HEXADECIMAL:
            validate = std::find_if(std::execution::par_unseq, number.begin(), number.end(), [](constValueTypeReference value)
            { return value < 0 || value > 15 || value < '0' || std::tolower(value) > 'f'; });
            return validate == number.end() ? true : false;

        default:
            return false;
        }
    }

    template<class T>
    T converter(BASE base)
    {
        T result;
        switch(base)
        {
        case BASE::BINARY:
            std::copy(m_number.begin(), m_number.end(), std::back_inserter(result));
            break;
        default:
            return result;
        }
    }

    // TODO: count bits
    BigInt(const std::deque<bool>& number, SIGN sign, size_t bits = 0):
        m_number{number},
        m_sign{sign},
        m_bitSet{bits}
    {}

public:

    template<typename Type, class = typename std::enable_if_t< is_allow_primary<Type>() > >
    BigInt(Type& value):
        m_number{toBinary(value)}
    {}

    template<typename Type, class = typename std::enable_if_t< is_allow_primary<Type>() > >
    BigInt(const Type& value):
        m_number{toBinary(value)}
    {}

    template<typename Type, class = typename std::enable_if_t< is_allow_primary<Type>() > >
    BigInt(Type&& value):
        m_number{toBinary(value)}
    {}

    template<typename Type, class = typename std::enable_if_t<is_allow_container<Type>()>>
    BigInt(Type& value, BASE base):
        m_number{toBinary(value, base)}
    {}

    template<typename Type, class = typename std::enable_if_t<is_allow_container<Type>()>>
    BigInt(const Type& value, BASE base):
        m_number{toBinary(value, base)}
    {}

    template<typename Type, class = typename std::enable_if_t<is_allow_container<Type>()>>
    BigInt(Type&& value, BASE base):
        m_number{toBinary(value, base)}
    {}

    template<typename Type, class = typename std::enable_if_t< is_allow_primary<Type>() > >
    BigInt(Type* value, size_t size, BASE base):
        m_number{toBinary(value, base)} // TODO
    {}

    template<typename Type, class = typename std::enable_if_t<is_allow_primary<Type>()>>
    BigInt(const std::initializer_list<Type>& value, BASE base):
        m_number{toBinary(value, base)}
    {}

    template<typename Type, class = typename std::enable_if_t<is_allow_primary<Type>()>>
    BigInt(std::initializer_list<Type>&& value, BASE base):
        m_number{toBinary(value, base)}
    {}

    BigInt():
        m_number{0},
        m_sign{SIGN::POSITIVE},
        m_bitSet{0}
    {}

    void Print() const
    {
        for(const auto& bit : m_number)
            std::cout << bit << std::endl;
    }

    template<class T, class = typename std::enable_if_t< is_allow_primary<T>()> >
    friend BigInt operator+(const BigInt& number, T other)
    {
        return number + BigInt(other);
    }

    friend BigInt operator+(const BigInt& lhs, const BigInt& rhs)
    {
        if(lhs.isNegative())
        {
            if(rhs.isPositiove())
                return rhs - lhs;

            return BigInt(addition(lhs, rhs), SIGN::POSITIVE);
        }
        else
        {
            if(lhs.isPositiove())
                return lhs - rhs;

            return BigInt(addition(lhs, rhs), SIGN::POSITIVE);
        }
    }

    template<class T, class = typename std::enable_if_t< is_allow_primary<T>()> >
    friend BigInt operator-(const BigInt& number, T other)
    {
        return number - BigInt(other);
    }

    friend BigInt operator-(const BigInt& rhs, const BigInt& lhs)
    {
        if(rhs.isNegative() && lhs.isNegative())
            return BigInt(addition(rhs, lhs), SIGN::POSITIVE);
        if(rhs < lhs)
        {
            auto result =  subtraction(lhs, rhs);
            return BigInt(result, SIGN::NEGATIVE);
        }
        else
        {
            auto result =  subtraction(rhs, lhs);
            return BigInt(result, SIGN::POSITIVE);
        }
    }

    friend BigInt operator*(const BigInt& rhs, const BigInt& lhs)
    {
        auto minus = [](std::deque<bool> a, std::deque<bool> b){
            std::string s1;
            std::string s2;

            for(auto v : a) s1 += std::to_string(v);
            for(auto v : b) s2 += std::to_string(v);

            return BigInt( std::strtoull(s1.c_str(), NULL, 2) - std::strtoull(s2.c_str(), NULL, 2)  );
        };

        auto plus = [](std::deque<bool> a, std::deque<bool> b){
            std::string s1;
            std::string s2;

            for(auto v : a) s1 += std::to_string(v);
            for(auto v : b) s2 += std::to_string(v);

            return BigInt( std::strtoull(s1.c_str(), NULL, 2) + std::strtoull(s2.c_str(), NULL, 2)  );
        };

        BigInt res;
        BigInt tmp = lhs;
        size_t offset = 0;
        auto handleZero = [&]()
        {
            if(offset >= 1)
            {
                std::cout << "distance " << offset << std::endl;
                res = plus(res.List(), tmp.List());
                if(offset > 1) res = minus(res.List(), lhs.List());
                offset = 0;
            }
        };

        for(auto it = crbegin(rhs.List()); it != crend(rhs.List()); ++it)
        {
            if(*it == 0) tmp <<= 1;
            if(*it == 1)
            {
                ++offset;
                if( *std::next(it, 1) == 0 || std::next(it, 1) == crend(rhs.List()) ) handleZero();
                tmp <<= 1;
            }
        }

        return res;
    }

    friend BigInt operator/(const BigInt& rhs, const BigInt& lhs)
    {
        return division(rhs, lhs);
    }

    friend bool operator >(const BigInt& rhs, const BigInt& lhs)
    {
        if(rhs.count() > lhs.count()) return true;
        else if(rhs.count() < lhs.count()) return false;

        for(auto rhs_it = begin(rhs.List()), lhs_it = begin(lhs.List()); rhs_it != end(rhs.List()); ++lhs_it, ++rhs_it)
            if(*rhs_it != *lhs_it && *rhs_it < *lhs_it)
                return false;

        return true;
    }

    friend bool operator <(const BigInt& rhs, const BigInt& lhs)
    {
        if(rhs.count() < lhs.count()) return true;
        else if(rhs.count() > lhs.count()) return false;

        for(auto rhs_it = begin(rhs.List()), lhs_it = begin(lhs.List()); rhs_it != end(rhs.List()); ++lhs_it, ++rhs_it)
            if(*rhs_it != *lhs_it && *rhs_it > *lhs_it)
                return false;

        return true;
    }

    friend bool operator >=(const BigInt& rhs, const BigInt& lhs)
    {
        return operator<(lhs, rhs);
    }

    friend bool operator <=(const BigInt& rhs, const BigInt& lhs)
    {
        return !(operator>(rhs, lhs));
    }

    friend bool operator ==(const BigInt& rhs, const BigInt& lhs)
    {
        if(rhs.count() != lhs.count()) return false;

        for(auto rhs_it = begin(rhs.List()), lhs_it = begin(lhs.List()); rhs_it != end(rhs.List()); ++lhs_it, ++rhs_it)
            if(*rhs_it != *lhs_it)
                return false;

        return true;
    }

    friend bool operator !=(const BigInt& rhs, const BigInt& lhs)
    {
        return !(operator==(rhs, lhs));
    }

    BigInt operator <<(size_t _shifts) const
    {
        std::deque<bool> newNumber = m_number;
        for(size_t i {0}; i < _shifts; ++i)
            newNumber.push_back(0);

        return BigInt(newNumber, SIGN::POSITIVE);
    }

    BigInt& operator <<=(size_t _shifts)
    {
        for(size_t i {0}; i < _shifts; ++i)
            m_number.push_back(0);
        return *this;
    }

    BigInt operator >>(size_t _shifts) const
    {
        if(m_number.size() < _shifts) return *this;

        std::deque<bool> newNumber = m_number;
        for(size_t i {0}; i < _shifts; ++i)
            newNumber.pop_back();

        return BigInt(newNumber, BASE::BINARY);
    }

    BigInt& operator >>=(size_t _shifts)
    {
        if(m_number.size() < _shifts) return *this;
        for(size_t i {0}; i < _shifts; ++i)
            m_number.pop_back();
        return *this;
    }

    BigInt operator-(const BigInt& number)
    {
        return BigInt(number.List(), number.isNegative() ? SIGN::POSITIVE : SIGN::NEGATIVE, number.count());
    }

    std::string Binary() const
    {
        std::string binary;
        binary.reserve(m_number.size());
        for(const auto& bit : m_number)
        {
            binary += static_cast<char>(bit);
        }

        return binary;
    }

    std::string Decimal() const
    {
        auto add = [](std::string& num1, const std::string& num2)
        {
            size_t remainder = 0;
            auto it_num1 = rbegin(num1);
            auto it_num2 = crbegin(num2);

            while(true)
            {
                if(it_num2 == crend(num2))
                {
                    assert(remainder < 10);

                    if(remainder != 0)
                    {
                        while(true)
                        {
                            if(it_num1 == rend(num1))
                            {
                                if(remainder != 0) num1.insert(0, std::to_string(remainder));
                                break;
                            }

                            int newDigit = (*it_num1-'0') + remainder;
                            *it_num1 = (newDigit % 10) + '0';
                            remainder = newDigit / 10;
                            ++it_num1;
                        }
                    }

                    break;
                }
                int digit1 = *it_num1 - '0';
                int digit2 = *it_num2 - '0';
                int newDigit = digit1 + digit2;
                assert(newDigit < 100);
                *it_num1 = newDigit % 10 + '0';
                remainder = newDigit / 10;
                ++it_num2;
                ++it_num1;
            }
        };
        std::string result = "0";
        std::string degree = "1";
        for(auto bit = m_number.crbegin(); bit != m_number.crend(); ++bit)
        {
            if(static_cast<int>(*bit) == 1) add(result, degree);
            add(degree, degree);
        }

        return result;
    }

    std::string Hex() const
    {
        std::unordered_map<std::string, char> binToHex = {
            {"0001", '1'},
            {"0010", '2'},
            {"0011", '3'},
            {"0100", '4'},
            {"0101", '5'},
            {"0110", '6'},
            {"0111", '7'},
            {"1000", '8'},
            {"1001", '9'},
            {"1010", 'A'},
            {"1011", 'B'},
            {"1100", 'C'},
            {"1101", 'D'},
            {"1110", 'E'},
            {"1111", 'F'},
        };

        auto makeChan = [](std::deque<bool>::const_reverse_iterator& it, std::deque<bool>::const_reverse_iterator& end) -> std::string
        {
            std::string chan;
            chan.reserve(4);

            for(int i = 0; i < 4; ++i, ++it)
            {
                if(it == end)
                {
                    std::string newChan = "0000";
                    for(short i = 3; i >= 0; --i)
                    {
                        newChan[i] = chan[i];
                    }
                    return newChan;
                }
                chan += static_cast<char>(*it);
            }

            return chan;
        };

        std::string hex;
        hex.reserve(m_number.size() / 4);

        for(auto it = crbegin(m_number), end = crend(m_number); it != end; ++it)
        {
            std::string chan = makeChan(it, end);
            hex += binToHex[chan];
        }

        return hex;
    }

    std::string Octal() const
    {
        std::unordered_map<std::string, char> binToOctal = {
            {"001", '1'},
            {"010", '2'},
            {"011", '3'},
            {"100", '4'},
            {"101", '5'},
            {"110", '6'},
            {"111", '7'},
        };

        auto makeChan = [](std::deque<bool>::const_reverse_iterator& it, std::deque<bool>::const_reverse_iterator& end) -> std::string
        {
            std::string chan;
            chan.reserve(3);

            for(int i = 0; i < 3; ++i, ++it)
            {
                if(it == end)
                {
                    std::string newChan = "000";
                    for(short i = 2; i >= 0; --i)
                    {
                        newChan[i] = chan[i];
                    }
                    return newChan;
                }
                chan += static_cast<char>(*it);
            }

            return chan;
        };

        std::string octal;
        octal.reserve(m_number.size() / 3);

        for(auto it = crbegin(m_number), end = crend(m_number); it != end; ++it)
        {
            std::string chan = makeChan(it, end);
            octal += binToOctal[chan];
        }

        return octal;
    }

    // rename to Data
    const std::deque<bool>& List() const
    {
        return m_number;
    }

    template<class T>
    std::vector<T> Vector(BASE base) const
    {
        return converter<std::vector<T>>(base);
    }

    template<class T>
    std::deque<T> Deque(BASE base) const
    {
        return converter<std::deque<T>>(base);
    }

    template<class T>
    std::forward_list<T> ForwarList(BASE base) const
    {
        return converter<std::forward_list<T>>(base);
    }

//    template<class T>
//    std::list<T> List(BASE base) const
//    {
//        return converter<std::list<T>>(base);
//    }

    bool is2Pow() const
    {
        return m_bitSet == 1;
    }

    std::size_t count() const
    {
        return m_bitSet;
    }

    std::size_t Bit() const
    {
        return m_number.size();
    }

    bool isZero() const
    {
        return m_number.size() == 1 && *(m_number.begin()) == 0;
    }

    bool isEven() const
    {
        return *crbegin(m_number) == 0;
    }

    bool isOdd() const
    {
        return !(isEven());
    }

    bool isPositiove() const
    {
        return m_sign == SIGN::POSITIVE ? true : false;
    }

    bool isNegative() const
    {
        return !isPositiove();
    }

};

} // namespace BigInt

//    bool compare(std::deque<bool>::const_iterator it, std::deque<bool>::const_iterator start, std::deque<bool>::const_iterator end)
//    {
//        for(; start != end; ++it, ++start, ++end)
//            if(*it != *start && *it > *start)
//                return true;

//        return false;
//    }

//    BigInt offsetByUnits(std::deque<bool>& number, size_t offset)
//    {
//        for(size_t i {0}; i < offset; ++i)
//            number.push_back(1);
//        return BigInt(number, SIGN::POSITIVE);
//    }

