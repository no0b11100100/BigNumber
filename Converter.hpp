#pragma once
#include <deque>
#include <unordered_map>
#include <array>
#include <string>
#include <initializer_list>
#include <list>
#include <forward_list>
#include <vector>
#include <algorithm>
#include <execution>
#include <cassert>

std::unordered_map<std::string, char> binaryCast
{
    {"000", '0'},
    {"001", '1'},
    {"010", '2'},
    {"011", '3'},
    {"100", '4'},
    {"101", '5'},
    {"110", '6'},
    {"111", '7'},

    {"0000", '0'},
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
// TODO: replace any where -> dubplicate in Operators.hpp
using Bit = bool;
using BinaryData = std::deque<Bit>;
using InitList = std::initializer_list<Bit>;
using ArrayList = std::array<InitList, 2>;

std::unordered_map<char, ArrayList> hexOctalCast
{
    { '0', { InitList{0,0,0,0}, {0,0,0} } },
    { '1', { InitList{0,0,0,1}, {0,0,1} } },
    { '2', { InitList{0,0,1,0}, {0,1,0} } },
    { '3', { InitList{0,0,1,1}, {0,1,1} } },
    { '4', { InitList{0,1,0,0}, {1,0,0} } },
    { '5', { InitList{0,1,0,1}, {1,0,1} } },
    { '6', { InitList{0,1,1,0}, {1,1,0} } },
    { '7', { InitList{0,1,1,1}, {1,1,1} } },
    { '8', { InitList{1,0,0,0}, {} } },
    { '9', { InitList{1,0,0,1}, {} } },
    { 'A', { InitList{1,0,1,0}, {} } },
    { 'B', { InitList{1,0,1,1}, {} } },
    { 'C', { InitList{1,1,0,0}, {} } },
    { 'D', { InitList{1,1,0,1}, {} } },
    { 'E', { InitList{1,1,1,0}, {} } },
    { 'F', { InitList{1,1,1,1}, {} } },
};
enum class Base
{
    Binary,
    Octal,
    Decimal,
    Hexadecimal,
};

template<typename T>
bool is_allow_primary()
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
bool is_allow_container()
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

struct Validator
{
    template<class T>
    bool validation(T&& value, Base base)
    {
        assert (static_cast<unsigned>(base) < static_cast<unsigned>(Base::Hexadecimal));
        unsigned validatedValue;
        if(std::is_same_v<std::decay_t<T>, char>)
        {
            if(base == Base::Hexadecimal)
            {
                if(value > '9')
                    validatedValue = static_cast<unsigned>(std::tolower(value) - 'a');
                else
                    validatedValue = static_cast<unsigned>(value);
            }
            else
            {
                validatedValue = static_cast<unsigned>(value);
            }
        }

        return validatedValue >= limits[base].first &&
                  validatedValue <= limits[base].second;
    }

    template< class TContainer>
    bool isValid(TContainer&& container, Base base)
    {
        if(is_allow_primary<TContainer>())
        {
            return validation(container, base);
        }
        else
        {
            using valueType = typename std::decay_t<TContainer>::value_type;
            return !is_allow_container<TContainer>() ? false :
            std::find_if_not(std::execution::par_unseq, cbegin(container), cend(container),
                                    [&](const valueType& value){ return validation(value, base); })
                    == container.cend();
        }
    }

    template<class T>
    bool operator()(T&& value, Base base)
    {
        return isValid(value, base);
    }

private:
    std::unordered_map<Base, std::pair<unsigned, unsigned>> limits {
        { Base::Binary, {0, 1} },
        { Base::Octal, {0, 7} },
        { Base::Decimal, {0, 9} },
        { Base::Hexadecimal, {0, 15} },
    };
} validator;

/**
 *  FromBinary convert BinaryData to container for set base
 */
class FromBinary
{
    constexpr static const unsigned short m_octalChanSize = 3;
    constexpr static const unsigned short m_HexChanSize = 4;

    template<size_t N, class ReversIterator>
    char castHexOrOctalToBinary(ReversIterator it, ReversIterator end)
    {
        static_assert (N == 3 || N == 4, "chan size must be 3 or 4");
        std::string chan(N, '0');

        for(auto chanIt = rbegin(chan); chanIt != chan.rend() || it != end; ++it, ++chanIt)
            *chanIt = *it-'0';

        return binaryCast[chan];
    }

    template<class TContainer, size_t chanSize>
    TContainer convertHexOrOctalToBinary(const BinaryData& binary)
    {
        TContainer result;
        result.reserve(binary.size() / chanSize);
        for(auto it = binary.crbegin(); it != binary.crend(); it += chanSize)
        {
            char symbol = castHexOrOctalToBinary<chanSize>(it, binary.crend());
            result.push_back(symbol);
        }

        return result;
    }

    template<class T>
    void aditionDecimal(std::deque<T>& result, const std::string& addendum)
    {
        assert(result.size() <= addendum.size());
        size_t transfer = 0;
        auto toInt = [](const auto& symbol) -> size_t { return static_cast<size_t>(symbol); };
        std::transform(result.rbegin(), result.rend(), addendum.crbegin(), result.rbegin(),
                       [&](const T& resultValue, const char* addendumValue) -> T
        {
            size_t newValue = toInt(resultValue) + toInt(*addendumValue) + transfer;
            transfer /= 10;
            return static_cast<T>(newValue % 10);
        });

        if(transfer != 0)
            result.push_front(static_cast<T>(transfer));

    }

    void multiplicationBy2(std::string& result)
    {
        size_t transfer = 0;
        auto charToInt = [](const char& symbol) -> size_t { return static_cast<size_t>(symbol); };
        auto intToChar = [](const size_t& number) -> char { return static_cast<char>(number); };
        std::transform(result.rbegin(), result.rend(), result.begin(),
                       [&transfer, &charToInt, &intToChar](const char& digit) -> char
        {
            size_t newValue = charToInt(digit)*2 + transfer;
            transfer /= 10;
            return intToChar(newValue % 10);
        });

        assert(transfer <= 9);
        if(transfer != 0)
            result.insert(intToChar(transfer), 0);
    }

public:

    template<class TContainer>
    TContainer ToBinary(const BinaryData& binary)
    {
        TContainer result;
        result.reserve(binary.size());
        std::copy(binary.crbegin(), binary.crend(), std::back_inserter(result));
        return result;
    }

    template<class TContainer>
    TContainer ToOctal(const BinaryData& binary)
    {
        return convertHexOrOctalToBinary<TContainer, m_octalChanSize>(binary);
    }

    template<class TContainer>
    TContainer ToDecimal(const BinaryData& binary)
    {
        using ValueType = typename std::decay_t<TContainer>::value_type;
        static_assert (std::is_same_v<ValueType, bool>,
                "for decimal number bool is not allowed, bool only for binary number");
        std::deque<ValueType> tempResult(0);
        tempResult.reserve(binary.size() / 5);
        std::string degree = "1";

        for(auto it = binary.crbegin(); it != binary.crend(); ++it)
        {
            if(*it == 1) additionDecimal(tempResult, degree);
            multiplicationBy2(degree);
        }

        TContainer result;
        result.reserve(tempResult.size());
        std::move(tempResult.begin(), tempResult.end(), result.begin());

        return result;
    }

    template<class TContainer>
    TContainer ToHex(const BinaryData& binary)
    {
        return convertHexOrOctalToBinary<TContainer, m_HexChanSize>(binary);
    }

} fromBinary;

/**
 *   ToBinary convert any collection to binary format
 */
class ToBinary
{
    constexpr static const unsigned short octalChanSize = 3;
    constexpr static const unsigned short hexChanSize = 4;

    template<size_t N, class ReversIterator>
    std::initializer_list<Bit> makeChan(char symbol)
    {
        static_assert (N == 3 || N == 4, "chan size must be 3 or 4");
        return hexOctalCast[symbol][hexChanSize-N];
    }

    template<size_t N, class TContainer>
    BinaryData convertHexOrOctalToBinary(TContainer&& container)
    {
        BinaryData result;

        if(is_allow_primary<TContainer>())
        {
            if(int value = static_cast<int>(container); validator(value, Base::Octal))
            {
                return BinaryData{makeChan<N>(value-'0')};
            }
        }

        for(auto const& symbol : container)
        {
            auto bitsList = makeChan<N>(symbol);
            std::move(bitsList.begin(), bitsList.end(), std::back_inserter(result));
        }

        return result;
    }

    /**
     * Division by 2
     * @param number
     * @return remainder
     */
    template<class TContainer>
    Bit getRemainder(TContainer&& number)
    {
        unsigned int remainder = 0;
        unsigned int dividend;
        unsigned int quot;
        using ValueTypeReference = typename std::decay_t<TContainer>::value_type;
        std::for_each(number.begin(), number.end(), [&](ValueTypeReference value)
        {
            dividend = (remainder * 10) + value;
            remainder = dividend & 1;
            quot = dividend / 2;
            value = quot;
        });

        return static_cast<Bit>(remainder);
    };

    template<class TContainer>
    BinaryData toBinary(TContainer&& container)
    {
        BinaryData binary;
        if(is_allow_primary<TContainer>())
        {
            if(unsigned bit = static_cast<unsigned>(container); bit <= 1)
                binary.push_back(bit);
        }
        else
            std::copy(container.cbegin(), container.cend(), std::back_inserter(binary));

        return binary;
    }

    template<class TContainer>
    BinaryData toOctal(TContainer&& container)
    {
        return convertHexOrOctalToBinary<octalChanSize>(std::forward<TContainer>(container));
    }

    template<class TContainer>
    BinaryData toDecimal(TContainer&& container)
    {
        BinaryData binary;
        if(is_allow_primary<TContainer>())
        {
            if(container == 0) binary.push_back(0);
            else
            {
                while(container > 0)
                {
                    binary.push_back(container&1);
                    container /= 2;
                }
            }
        }
        else
        {
            while(!container.empty())
            {
                Bit bit = getRemainder(std::forward<TContainer>(container));
                binary.push_front(bit); // TODO: clarify
            }
        }

        return binary;
    }

    template<class TContainer>
    BinaryData toHex(TContainer&& container)
    {
        return convertHexOrOctalToBinary<hexChanSize>(std::forward<TContainer>(container));
    }

public:

    template<class TContaimner>
    BinaryData operator()(TContaimner&& container, Base base = Base::Decimal)
    {
        if(!validator(container, base))
        {
            return {};
        }
        switch(base)
        {
        case Base::Binary:
            return toBinary(container);
        case Base::Octal:
            return toOctal(container);
        case Base::Decimal:
            return toDecimal(container);
        case Base::Hexadecimal:
            return toHex(container);
        default:
            return {};
        }
    }

} toBinary;
