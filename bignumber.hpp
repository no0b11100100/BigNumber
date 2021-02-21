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

        if(!(number > 0)) {
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

    struct Minus
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
    };

    void offsetByUnits()
    {
        m_number.push_back(1);
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

    friend BigInt operator+(const BigInt& rhd, const BigInt& lhd)
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

        return BigInt(result, SIGN::POSITIVE);
    }

    template<class T, class = typename std::enable_if_t< is_allow_primary<T>()> >
    friend BigInt operator-(const BigInt& number, T other)
    {
        return number - BigInt(other);
    }

    friend BigInt operator-(const BigInt& rhs, const BigInt& lhs)
    {
        Minus m;
        if(rhs < lhs)
        {
            auto result =  m(lhs, rhs);
            return BigInt(result, SIGN::NEGATIVE);
        }
        else
        {
            auto result =  m(rhs, lhs);
            return BigInt(result, SIGN::POSITIVE);
        }

    }

    BigInt operator-=(const BigInt& other)
    {
        return operator-(*this, other);
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
        size_t _distance = 0;
        auto handleZero = [&]()
        {
            if(_distance >= 1)
            {
                std::cout << "distance " << _distance << std::endl;
                res = plus(res.List(), tmp.List());
                if(_distance > 1) res = minus(res.List(), lhs.List());
                _distance = 0;
            }
        };

        for(auto it = rbegin(rhs.List()); it != rend(rhs.List()); ++it)
        {
            if(*it == 1)
            {
                ++_distance;
                if(*(std::next(it, 1)) == 0 && std::next(it, 1) != rend(rhs.List()) )
                {
                    handleZero();
                }
                tmp <<= 1;
            }
            else if(*it == 0)
            {
                tmp <<= 1;
            }
        }

        tmp >>= 1;
        handleZero();

        std::cout << "tmp\n";
        for(auto v : res.List())
            std::cout << v << " ";
        std::cout << "tmp end\n";

        return res;
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
        return !(operator<(rhs, lhs));
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

        return BigInt(newNumber, BASE::BINARY);
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

    const std::deque<bool>& List() const
    {
        return m_number;
    }

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

    void ChangeSign()
    {
        m_sign = m_sign == SIGN::POSITIVE ? SIGN::NEGATIVE : SIGN::POSITIVE;
    }

};

} // namespace BigInt
