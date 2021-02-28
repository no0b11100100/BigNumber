#pragma once
#include <deque>
#include <unordered_map>
#include <array>
#include <string>
#include <initializer_list>

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
// TODO: replace any where -> dubplicate in OPerators.hpp
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
    { '8', { InitList{0,0,0,0}, {} } },
    { '9', { InitList{0,0,0,0}, {} } },
    { 'A', { InitList{0,0,0,0}, {} } },
    { 'B', { InitList{0,0,0,0}, {} } },
    { 'C', { InitList{0,0,0,0}, {} } },
    { 'D', { InitList{0,0,0,0}, {} } },
    { 'E', { InitList{0,0,0,0}, {} } },
    { 'F', { InitList{0,0,0,0}, {} } },
};

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

public:
    template<class TContainer>
    BinaryData toBinary(TContainer&& container)
    {
        BinaryData binary;
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
        while(!container.empty())
        {
            Bit bit = getRemainder(std::forward<TContainer>(container));
            binary.push_front(bit); // TODO: clarify
        }

        return binary;
    }

    template<class TContainer>
    BinaryData toHex(TContainer&& container)
    {
        return convertHexOrOctalToBinary<hexChanSize>(std::forward<TContainer>(container));
    }

} toBinary;
