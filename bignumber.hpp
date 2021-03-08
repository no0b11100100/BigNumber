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
using remove_all_t = typename std::decay_t<remove_all_pointers<T>>;

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
    using ValueType = typename std::decay_t<Container>::value_type;
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


class BigInt
{
    std::list<bool> m_number;
    bool sign;
    std::size_t setedBits;

    void toBinary(int number)
    {
        while(number > 0)
        {
            m_number.push_front(static_cast<bool>(number % 2));
            number >>= 1;
        }
    }

    std::list<bool> toBinary(std::size_t number, int)
    {
        std::list<bool> binary;
        while(number > 0)
        {
            binary.push_front(static_cast<bool>(number % 2));
            number >>= 1;
        }

        return binary;
    }

    std::list<bool> toBinary(std::size_t& number, int) const
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

    static std::size_t toLow2Pow(std::size_t number)
    {
        --number;
        number |= number >> 1;
        number |= number >> 2;
        number |= number >> 4;
        number |= number >> 8;
        number |= number >> 16;
        number |= number >> 32;
        number >>= 1;
        return ++number;
    }

    static bool is2Pow(std::size_t number)
    {
        return toLow2Pow(number) == number;
    }

public:
    // https://godbolt.org/z/1rb8M1
    template<typename Type, class = typename std::enable_if_t< is_allow_primary<Type>() > >
    BigInt(Type&){}

    template<typename Type, class = typename std::enable_if_t< is_allow_primary<Type>() > >
    BigInt(const Type&){}

    template<typename Type, class = typename std::enable_if_t< is_allow_primary<Type>() > >
    BigInt(Type&&){}

    template<typename Type, class = typename std::enable_if_t< is_allow_container<Type>() > >
    BigInt(Type&, BASE base){}

    template<typename Type, class = typename std::enable_if_t< is_allow_container<Type>() > >
    BigInt(const Type&, BASE base){}

    template<typename Type, class = typename std::enable_if_t< is_allow_container<Type>() > >
    BigInt(Type&&, BASE base){}

    BigInt(const BigInt&) = default;
    BigInt(BigInt&&) = default;

    BigInt& operator=(const BigInt&) = default;
    BigInt& operator=(BigInt&&) = default;

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
        std::unordered_map<std::string, char> binToHex = {
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
            octal += binToHex[chan];
        }

        return octal;
    }

    void Print() const
    {
        for(const auto& bit : m_number)
        {
            std::cout << bit << " ";
        } std::cout << std::endl;
    }

    std::size_t toLow2Pow() const
    {
        return m_number.size();
    }

    void DecrementFromIndex(std::size_t s)
    {
        for(auto it = std::next(m_number.rbegin(), s); it != m_number.rend(); ++it)
        {
            *it = 0;
            if(*it == 1) break;
        }

        removeZeros(m_number);
    }

    void IncrementFromIndex(std::size_t s)
    {
        for(auto it = std::next(m_number.rbegin(), s); it != m_number.rend(); ++it)
        {
            *it = 1;
            if(*it == 0) break;
        }
    }

    // TODO: fix (tested on number 32)
    bool is2Pow() const
    {
        auto head_start = std::next(begin(m_number),1);
        auto end_start = rbegin(m_number);
        for(; head_start != end_start.base(); ++head_start, ++end_start)
        {
            if(*head_start != 0 || *end_start != 0) return false;
        }
        return true;
    }

    bool isZero() const
    {
        return m_number.size() == 1 && *(m_number.begin()) == 0;
    }

    // http://oldskola1.narod.ru/Kiselev07/K07.htm +
    // https://www.geeksforgeeks.org/long-division-method-to-find-square-root-with-examples/
    std::tuple<bool, BigInt> isPerfectSquare() const
    {
        std::list<bool> temp;
        temp.push_front(1);
        for(std::size_t i = 0; i < toLow2Pow()-1; ++i) temp.push_back(0);

        BigInt b(temp);
        BigInt x;
        BigInt n = *this;

        while(!b.isZero())
        {
            if(n >= x + b)
            {
                n = n - x - b;
                x = (x>>1) + b;
            }
            else
            {
                x >>= 1;
            }

            b >>= 2;
        }

        return {false, x};
    }

    // https://ideone.com/G9T0Lr
    // https://ru.stackoverflow.com/questions/54193/%D0%AF%D0%B2%D0%BB%D1%8F%D0%B5%D1%82%D1%81%D1%8F-%D0%BB%D0%B8-%D1%87%D0%B8%D1%81%D0%BB%D0%BE-%D0%BF%D1%80%D0%BE%D1%81%D1%82%D1%8B%D0%BC
    bool isPrime() const
    {
        bool notPrime = m_number.size() == 1 && (*(m_number.begin()) == 0 || *(m_number.begin()) == 1); // 0,1
        bool prime = !notPrime && m_number.size() == 2 &&
                ( (*(m_number.begin()) == 1 && *(std::next(m_number.begin(), 1)) == 0) // 2
                 || ((*(m_number.begin()) == 1 && *(std::next(m_number.begin(), 1)) == 1)) ); // 3
        if(notPrime) return false;
        if(prime) return true;

        if ((Pow(*this, 2)) % 24 == 1)
        {
            return true;
        }

        return false;
    }

    std::size_t SetedBits() const
    {
        return setedBits;
    }

    std::list<bool> List() const
    {
        return m_number;
    }

    bool isEven() const
    {
        return !m_number.empty() && *(m_number.rbegin()) == 0;
    }

    bool isOdd() const
    {
        return !isEven();
    }

    void Abs()
    {
        if(sign == false) sign = true;
    }

    bool isPositive() const
    {
        return sign == true ? true : false;
    }

    bool isNegative() const
    {
        return !isPositive();
    }

    void ChangeSign()
    {
        sign = !sign;
    }

    // https://prog-cpp.ru/simple-multy/
    // https://brestprog.by/topics/factorization/ +
    // https://ideone.com/OMcMN6 - C++ impl
    // https://ideone.com/oSZsIG - C impl
    // https://ideone.com/X2govK - C++ impl
    std::list<BigInt> Factorize() const
    {
//        list<BigInt> factors;
//        for (std::size_t i = 2; i <= sqrt(x); ++i) {
//                while (x % i == 0) {
//                    factors.push_back(i);
//                    x /= i;
//                }
//            }

//            if (x != 1) {
//                factors.push_back(x);
//            }

//            return factors;
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

    BigInt operator % (int)
    {
        return BigInt();
    }

    BigInt operator * (int number)
    {
        return this->operator*(toBinary(number, 1));
    }

    // https://ideone.com/IfJVQX
    BigInt operator * (std::list<bool> number)
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

    BigInt operator * (BigInt other) // need test
    {
        auto number = other.List();
        auto Number = *this;
        auto it = rbegin(number);
        while(true)
        {
            if(it == rend(number))
            {
//                *this <<= 1;
                *this = *this - Number;
                break;
            }
            if(*it == 0)
            {
                *this <<= 1;
                *this = *this - Number;
                ++it;
                continue;
            }

            *this <<= 1;
            ++it;
        }

        return *this;
    }

    BigInt operator - (int number)
    {
        return this->operator-(toBinary(number, 1));
    }

    BigInt operator - (std::list<bool> number)
    {

        std::string num_1 = "";
        std::string num_2 = "";

        for(const auto& bit : m_number) num_1 += bit + '0';
        for(const auto& bit : number) num_2 += bit + '0';

        auto res = toBinary( (std::strtol(num_1.c_str(), NULL, 2) - std::strtol(num_2.c_str(), NULL, 2)), 1);
        return BigInt(res);

//        auto self_it = rbegin(m_number);
//        auto number_it = rbegin(number);
//        std::list<bool> newNumber;
//        std::size_t size = std::max(m_number.size(), number.size());
//        bool isTransfer = false;

//        for(std::size_t i = 0; i < size; ++self_it, ++number_it, ++i)
//        {
//            if(isTransfer) *self_it = 0;

//            if(self_it == rend(m_number))
//            {
//                for(; number_it != rend(number); ++number_it)
//                {
//                    if(isTransfer)
//                    {
//                        if(*number_it == 0)  newNumber.push_front(1);
//                        else
//                        {
//                            isTransfer = false;
//                            newNumber.push_front(0);
//                        }
//                    }
//                    else newNumber.push_front(*number_it);
//                }
//                break;
//            } else if(number_it == rend(number))
//            {
//                for(; self_it != rend(m_number); ++self_it)
//                {
//                    if(isTransfer)
//                    {
//                        if(*self_it == 0)  newNumber.push_front(1);
//                        else
//                        {
//                            isTransfer = false;
//                            newNumber.push_front(0);
//                        }
//                    }
//                    else newNumber.push_front(*self_it);
//                }

//                if(*self_it == *number_it)
//                {
//                    newNumber.push_front(*self_it);
//                } else
//                {
//                    if(*self_it > *number_it) newNumber.push_front(*self_it);
//                    else
//                    {
//                        isTransfer = true;
//                        newNumber.push_front(1);
//                    }
//                }
//            }
//        }

//        removeZeros(m_number);

//        assert(*(newNumber.begin()) == 1);

//        return BigInt(newNumber);
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

    BigInt operator + (std::list<bool> number) // test and fix
    {
        std::string num_1 = "";
        std::string num_2 = "";

        for(const auto& bit : m_number) num_1 += bit + '0';
        for(const auto& bit : number) num_2 += bit + '0';

        auto res = toBinary( (std::strtol(num_1.c_str(), NULL, 2) + std::strtol(num_2.c_str(), NULL, 2)), 1);
        return BigInt(res);

//        auto self_it = rbegin(m_number);
//        auto number_it = rbegin(number);
//        std::list<bool> newNumber;
//        std::size_t size = std::max(m_number.size(), number.size());
//        bool isTransfer = false;

//        for(std::size_t i = 0; i < size; ++self_it, ++number_it, ++i)
//        {
//            if(self_it == rend(m_number))
//            {
//                for(; number_it != rend(number); ++number_it)
//                {
//                    if(isTransfer)
//                    {
//                        if(*number_it == 1)  newNumber.push_front(0);
//                        else
//                        {
//                            isTransfer = false;
//                            newNumber.push_front(1);
//                        }
//                    }
//                    else newNumber.push_front(*number_it);
//                }
//                break;
//            } else if(number_it == rend(number))
//            {
//                for(; self_it != rend(m_number); ++self_it)
//                {
//                    if(isTransfer)
//                    {
//                        if(*self_it == 1)  newNumber.push_front(0);
//                        else
//                        {
//                            isTransfer = false;
//                            newNumber.push_front(1);
//                        }
//                    }
//                    else newNumber.push_front(*self_it);
//                }
//                break;
//            }

//            if(*self_it == 1 && *number_it == 1)
//            {
//                isTransfer = true;
//                newNumber.push_front(0);
//            } else if(*self_it == 0 && *number_it == 0)
//            {
//                isTransfer ? newNumber.push_front(1) : newNumber.push_front(0);
//                isTransfer = false;
//            } else if(*self_it != *number_it)
//            {
//                if(!isTransfer) newNumber.push_front(1);
//                else
//                {
//                    newNumber.push_front(1);
//                    isTransfer = true;
//                }
//            }
//        }

//        if(isTransfer) newNumber.push_front(1);

////        for(auto v : newNumber) std::cout << v << std::endl;

//        assert(*(newNumber.begin()) == 1);

//        return BigInt(newNumber);
    }

    BigInt& operator += (BigInt other)
    {
        *this = operator+(other);
        return *this;
    }

    BigInt operator ~()
    {
        sign = !sign;
        auto newNumber = m_number;
        for(auto& bit : newNumber)
            bit = bit == 1 ? 0 : 1;

        return (++BigInt(newNumber));
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
        shift(m_number, shifts, true);
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
        shift(m_number, shifts);
        return *this;
    }

    BigInt& operator --()
    {
        for(auto it = m_number.rbegin(); it != m_number.rend(); ++it)
        {
            *it = 0;
            if(*it == 1) break;
        }

        removeZeros(m_number);

        return *this;
    }

    BigInt operator --(int)
    {
        auto number = *this;
        --(*this);
        return number;
    }

    BigInt& operator ++()
    {
        for(auto it = m_number.rbegin(); it != m_number.rend(); ++it)
        {
            *it = 1;
            if(*it == 0) break;
        }

        if(*(m_number.begin()) == 0) m_number.push_front(1);

        return *this;
    }

    BigInt operator ++(int)
    {
        auto number = *this;
        ++(*this);
        return number;
    }

    bool operator > (BigInt other) const
    {
//        if(m_number.size() > other.List().size()) return true;
//        if(m_number.size() < other.List().size()) return false;

//        assert(m_number.size() == other.List().size());

//        auto value = other.List();

//        for(std::list<bool>::const_iterator it = m_number.cbegin(), it_1 = value.cbegin(); it != m_number.cend(); ++it, ++it_1)
//        {
//            if(*it != *it_1) return *it < *it_1 ? false : true;
//        }

//        return false;
        std::string num_1 = "";
        std::string num_2 = "";
        auto number = other.List();
        for(const auto& bit : m_number) num_1 += bit + '0';
        for(const auto& bit : number) num_2 += bit + '0';

        return std::strtol(num_1.c_str(), NULL, 2) > std::strtol(num_2.c_str(), NULL, 2);
    }

    bool operator < (BigInt other) const
    {
        if(m_number.size() < other.List().size()) return true;
        if(m_number.size() > other.List().size()) return false;

        assert(m_number.size() == other.List().size());

        auto value = other.List();

        for(std::list<bool>::const_iterator it = m_number.cbegin(), it_1 = value.cbegin(); it != m_number.cend(); ++it, ++it_1)
        {
            if(*it != *it_1) return *it > *it_1 ? false : true;
        }

        return false;
    }

    bool operator >= (BigInt other)
    {
        std::string num_1 = "";
        std::string num_2 = "";
        auto number = other.List();
        for(const auto& bit : m_number) num_1 += bit + '0';
        for(const auto& bit : number) num_2 += bit + '0';

        return std::strtol(num_1.c_str(), NULL, 2) >= std::strtol(num_2.c_str(), NULL, 2);
//        return !(operator<(other));
    }

    bool operator <= (BigInt other)
    {
        return !(operator>(other));
    }

    bool operator == (BigInt other) const
    {
        if(m_number.size() != other.List().size()) return false;

        assert(m_number.size() == other.List().size());

        auto value = other.List();

        for(auto it = m_number.cbegin(), it_1 = value.cbegin(); it != m_number.cend(); ++it, ++it_1)
        {
            if(*it != *it_1) return false;
        }

        return true;
    }

    bool operator != (BigInt other)
    {
        return !(operator==(other));
    }

    static BigInt Pow(const BigInt& number, std::size_t pow) // TODO: handle if pow is 2 degree
    {
        BigInt result = std::move(number);
        std::size_t degree = toLow2Pow(pow);
        auto pow2 = [](std::size_t pow) -> std::size_t
        {
            std::size_t count = 0;
            while(pow)
            {
                pow >>= 1;
                ++count;
            }

            return --count;
        };

        for(std::size_t i = 0; i < pow2(degree); ++i)
            result = result * result;

        for(std::size_t i = 0; i < pow-degree; ++i)
            result = result * number;

        return result;
    }

    // https://en.wikipedia.org/wiki/Binary_GCD_algorithm
    static BigInt gcd(BigInt a, BigInt b)
    {
        auto isZero = [](std::list<bool> number) -> bool
        {
            return number.size() == 1 && *(number.begin()) == 0;
        };

        if(a == b) return a;
        if(isZero(a.List())) return b;
        if(isZero(b.List())) return a;

        if(a.isEven())
        {
            if(b.isOdd())
            {
                return gcd(a>>1, b);
            } else
            {
                return gcd(a>>1, b>>1) << 1;
            }
        }
        else
        {
            if(b.isEven())
            {
                return gcd(a, b>>1);
            }

            if(a > b)
            {
                return gcd((a-b) << 1, b);
            }
            else
            {
                return gcd((b-a) << 1, a);
            }
        }
    }

    static BigInt lcm(BigInt a, BigInt b)
    {
        return (a*b) / gcd(a,b);
    }

    static BigInt factorial(std::size_t n)
    {
        if(n == 1 || n == 0) return BigInt(1);

        bool handleOdd = false;
        auto uptoNumber = n;

        if((n & 1) == 1)
        {
            --uptoNumber;
            handleOdd = true;
        }

        BigInt nextSum(uptoNumber);
        BigInt nextMulti(uptoNumber);
        BigInt factorial(1);

        while(nextSum >= 2)
        {
            factorial = factorial * nextMulti;
            nextSum -= 2;
            nextMulti += nextSum;
        }

        if (handleOdd) factorial = factorial * n;

        return factorial;
    }

    // // https://medium.com/@dimko1/%D0%B0%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC%D1%8B-%D0%BD%D0%B0%D1%85%D0%BE%D0%B6%D0%B4%D0%B5%D0%BD%D0%B8%D0%B5-%D1%87%D0%B8%D0%BB%D1%81%D0%B0-%D1%84%D0%B8%D0%B1%D0%BE%D0%BD%D0%B0%D1%87%D1%87%D0%B8-b68a582b6374
    static BigInt fibinachi(std::size_t n)
    {
        return BigInt();
    }
};

} // namespace BigInt
