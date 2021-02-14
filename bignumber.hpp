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
#include <climits>
#include <exception>

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

    template<class T>
    std::list<bool> toBinary(T&& number)
    {
        std::list<bool> list_number;
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
    std::list<bool> toBinary(T&& number, BASE base)
    {
        if(!number.empty())
        {
            if( *(number.begin()) > 0) m_sign = SIGN::POSITIVE;
            else m_sign = SIGN::NEGATIVE;
        }

        std::list<bool> list_number;

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

    friend BigInt operator+(const BigInt& number1, const BigInt& number2)
    {
        auto number1_it = rbegin(number1.List());
        auto number2_it = rbegin(number2.List());
        std::list<bool> result;
        std::size_t size = std::max(number1.count(), number2.count());
        bool isTransfer = false;

        for(std::size_t i = 0; i < size; ++number1_it, ++number2_it, ++i)
        {
            if(number1_it == rend(number1.List()))
            {
                for(; number2_it != rend(number2.List()); ++number2_it)
                {
                    if(isTransfer)
                    {
                        if(*number2_it == 1) result.push_front(0);
                        else
                        {
                            isTransfer = false;
                            result.push_front(1);
                        }
                    }
                    else result.push_front(*number2_it);
                }
                break;
            } else if(number2_it == rend(number2.List()))
            {
                for(; number1_it != rend(number1.List()); ++number1_it)
                {
                    if(isTransfer)
                    {
                        if(*number1_it == 1)  result.push_front(0);
                        else
                        {
                            isTransfer = false;
                            result.push_front(1);
                        }
                    }
                    else result.push_front(*number1_it);
                }
                break;
            }

            if(*number1_it == 1 && *number2_it == 1)
            {
                isTransfer = true;
                result.push_front(0);
            } else if(*number1_it == 0 && *number2_it == 0)
            {
                isTransfer ? result.push_front(1) : result.push_front(0);
                isTransfer = false;
            } else if(*number1_it != *number2_it)
                result.push_front(1);
        }

        result.push_front(0); // sign
        return BigInt(result, BASE::BINARY);
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
        auto add = [](std::string& a, const std::string& b){
            // TODO: count max size of ULL in binary
            if(a.size() < std::numeric_limits<std::size_t>::max() && b.size() < std::numeric_limits<std::size_t>::max())
            {
                a = std::to_string(std::strtoull(a.c_str(), NULL, 2) + std::strtoull(b.c_str(), NULL, 2));
            }
            else
            {
                // TODO
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

        auto makeChan = [](std::list<bool>::const_reverse_iterator& it, std::list<bool>::const_reverse_iterator& end) -> std::string
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

        auto makeChan = [](std::list<bool>::const_reverse_iterator& it, std::list<bool>::const_reverse_iterator& end) -> std::string
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

    const std::list<bool>& List() const
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

    bool isZero() const
    {
        return m_number.size() == 1 && *(m_number.begin()) == 0;
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
