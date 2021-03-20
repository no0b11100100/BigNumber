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
#include <functional>
#include <iostream>

enum class Sign : bool
{
    Positive,
    Negative
};

std::unordered_map<std::string, char> fromBinaryTo
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

struct State
{
    State(const BinaryData& number, size_t bits = 0, Sign sign = Sign::Positive) :
        number{number},
        bitSet{bits},
        sign{sign}
    {}

    State() :
        number{0},
        bitSet{0},
        sign{Sign::Positive}
    {}

    BinaryData number;
    std::size_t bitSet;
    Sign sign;
};

template<typename T>
constexpr bool is_integer()
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
            std::is_same_v<Type, uint8_t> ||
            std::is_same_v<Type, uint16_t> ||
            std::is_same_v<Type, uint32_t> ||
            std::is_same_v<Type, uint64_t> ||
            std::is_same_v<Type, int8_t> ||
            std::is_same_v<Type, int16_t> ||
            std::is_same_v<Type, int32_t> ||
            std::is_same_v<Type, int64_t> ||
            std::is_same_v<Type, size_t>;
}

struct Validator
{
    template<class T>
    bool operator()(T&& value)
    {
        return isValid(std::forward<T>(value));
    }

private:

    static constexpr const char min = '0';
    static constexpr const char max = '9';

    bool isValid(const std::string& number)
    {
        auto it = *number.cbegin() == '-' ? std::next(number.cbegin()) : number.cbegin();
        auto validate = [min=min, max=max](const char& symbol) -> bool { return symbol >= min && symbol <= max; };
        return std::all_of(it, number.cend(), validate);
    }

    template< class T >
    bool isValid(T&&) { return true; }

} validator;

/**
 *  FromBinary convert BinaryData to container for set base
 */
class FromBinary
{
    constexpr static const unsigned short m_octalChanSize = 3;
    constexpr static const unsigned short m_HexChanSize = 4;

    template<class T>
    static char toChar(const T& number) { return number+'0'; };
    static unsigned charToInt(const char& symbol) { return symbol-'0'; };

    template<size_t N, class ReversIterator>
    static char castHexOrOctalToBinary(ReversIterator& it, ReversIterator end)
    {
        static_assert (N == 3 || N == 4, "chan size must be 3 or 4");
        std::string chan(N, '0');

        for(auto chanIt = chan.rbegin(); chanIt != chan.rend() && it != end; ++chanIt, ++it)
            *chanIt = toChar(*it);

        return fromBinaryTo[chan];
    }

    template<size_t chanSize>
    static std::string convertHexOrOctalToBinary(const BinaryData& binary)
    {
        std::deque<char> tempResult(binary.size() / chanSize);
        for(auto it = binary.crbegin(); it != binary.crend();)
        {
            char symbol = castHexOrOctalToBinary<chanSize>(it, binary.crend());
            tempResult.push_front(symbol);
        }

        std::string result;
        std::move(tempResult.cbegin(), tempResult.cend(), std::back_inserter(result));
        return result;
    }

    static void aditionDecimal(std::deque<char>& result, const std::string& addendum)
    {
        if(result.empty()) result.push_back('0');
        assert(result.size() <= addendum.size());
        unsigned transfer = 0;
        auto predicate = [&](const char& resultValue, const char& addendumValue = '0') -> char
        {
            assert(transfer <= 9);
            unsigned newValue = charToInt(resultValue) + charToInt(addendumValue) + transfer;
            assert(newValue < 100);
            transfer = newValue / 10;
            return toChar(newValue % 10);
        };

        std::transform(result.rbegin(), result.rend(), addendum.crbegin(), result.rbegin(), predicate);
        std::transform(std::next(addendum.crbegin(), result.size()), addendum.crend(), std::front_inserter(result), predicate);

        assert(transfer <= 9);
        if(transfer != 0)
            result.push_front(toChar(transfer));
    }

    static void multiplicationBy2(std::string& result)
    {
        unsigned transfer = 0;
        std::transform(result.rbegin(), result.rend(), result.rbegin(),
                       [&](const char& digit) -> char
        {
            assert(transfer <= 9);
            unsigned newValue = charToInt(digit)*2 + transfer;
            transfer = newValue / 10;
            return toChar(newValue % 10);
        });

        assert(transfer <= 9);
        if(transfer != 0)
            result.insert(result.cbegin(), toChar(transfer));
    }

    static void toAdditionalCode(std::string& result, const BinaryData& binary)
    {

    }

public:
    static std::string ToBinary(const BinaryData& binary, Sign sign)
    {
        std::string result;
        result.reserve(binary.size());
        if(sign == Sign::Positive)
        {
            std::copy(binary.crbegin(), binary.crend(), result.rbegin());
            return result;
        }
        else
        {
            bool isIncremented{false};
            std::transform(binary.crbegin(), binary.crend(), result.rbegin(), [&isIncremented](const Bit& bit) -> Bit
            {
                Bit newBit = bit == 1 ? 0 : 1;
                if(isIncremented == false && newBit == 0)
                {
                    isIncremented = true;
                    return static_cast<Bit>(1);
                }
                if(isIncremented == false && newBit == 1)
                    return static_cast<Bit>(0);

                if(isIncremented == true)
                    return newBit;
            });
            return '1' + result;
        }
    }

    static std::string ToOctal(const BinaryData& binary)
    {
        return convertHexOrOctalToBinary<m_octalChanSize>(binary);
    }

    static std::string ToDecimal(const BinaryData& binary)
    {
        std::deque<char> tempResult;
        std::string degree = "1";

        for(auto it = binary.crbegin(); it != binary.crend(); ++it)
        {
            if(*it == 1) aditionDecimal(tempResult, degree);
            multiplicationBy2(degree);
        }

        std::string result(tempResult.size(), '0');
        std::move(tempResult.begin(), tempResult.end(), result.begin());
        return result;
    }

    template<class T>
    static T toInt(const BinaryData& binary)
    {
        size_t degree = 1;
        T result = 0;
        for(auto it = binary.crbegin(); it != binary.crend(); ++it)
        {
            if(*it == 1) result += degree;
            degree *= 2;
        }

        return result;
    }

    static std::string ToHex(const BinaryData& binary)
    {
        return convertHexOrOctalToBinary<m_HexChanSize>(binary);
    }

};

/**
 *   ToBinary convert any collection to binary format
 */
class ToBinary
{
    /**
     * Division by 2
     * @param number
     * @return remainder
     */
    static Bit getRemainder(std::string& number, size_t& offset)
    {
        unsigned int remainder = 0;
        unsigned int dividend;
        unsigned int quot;

        offset = number.find_first_not_of('0', offset);
        auto it = std::next(number.begin(), offset);
        std::transform(it, number.end(), it, [&](const char& value)
        {
            dividend = (remainder * 10) + (value-'0');
            remainder = dividend & 1;
            quot = dividend / 2;
            return (quot+'0');
        });

        if(offset+1 == number.size() && *number.rbegin() == '0')
            ++offset;

        return static_cast<Bit>(remainder);
    }

    static State fromDecimal(std::string number)
    {
        BinaryData binary;
        size_t bits{0};
        size_t offset{0};
        while(offset != number.size())
        {
            Bit bit = getRemainder(number, offset);
            if(bit == 1) ++bits;
            binary.push_front(bit);
        }

        return State(binary, bits, *number.begin() == '-' ?
                                   Sign::Negative :
                                   Sign::Positive);
    }

    template<class T>
    static State fromDecimal(T number)
    {
        BinaryData binary;
        size_t bits{0};
        if(number == 0) return State();
        while(number > 0)
        {
            if(number&1) ++bits;
            binary.push_front(number&1);
            number /= 2;
        }

        return State(binary, bits, number < 0 ? Sign::Negative : Sign::Positive);
    }

public:

    template<class T>
    static State convert(T&& value)
    {
        return validator(value) ? fromDecimal(value) : State();
    }

};
