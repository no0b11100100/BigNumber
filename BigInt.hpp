#pragma once

#include <ostream>
#include <iostream>
#include <deque>
#include <execution>
#include <algorithm>
#include <climits>
#include <cassert>
#include <string>
#include <functional>
#include <cmath>
#include <iterator>

namespace
{

using Bit = bool;
using BinaryData = std::deque<Bit>;
using BinaryReturnType = std::tuple<BinaryData, size_t>;

template<class Iterator>
using ReturnType = std::tuple<BinaryData, Iterator, bool>;

enum class Sign : bool
{
    Positive,
    Negative
};

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
            std::is_same_v<Type, short> ||
            std::is_same_v<Type, unsigned short> ||
            std::is_same_v<Type, long> ||
            std::is_same_v<Type, unsigned long> ||
            std::is_same_v<Type, long long> ||
            std::is_same_v<Type, unsigned long long> ||
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

template<class T>
constexpr bool is_unsigned()
{
    using Type = std::decay_t<T>;
    return std::is_same_v<Type, unsigned> ||
            std::is_same_v<Type, unsigned short> ||
            std::is_same_v<Type, unsigned long> ||
            std::is_same_v<Type, unsigned long long> ||
            std::is_same_v<Type, uint8_t> ||
            std::is_same_v<Type, uint16_t> ||
            std::is_same_v<Type, uint32_t> ||
            std::is_same_v<Type, uint64_t> ||
            std::is_same_v<Type, size_t>;

}

namespace Operation
{

std::unordered_map<char, std::function<Bit(const Bit&, const Bit&)>> predicates
{
    { '^', [](const Bit& lhsBit, const Bit& rhsBit) { return lhsBit == rhsBit ?      0 : 1; } },
    { '|', [](const Bit& lhsBit, const Bit& rhsBit) { return lhsBit == rhsBit ? lhsBit : 1; } },
    { '&', [](const Bit& lhsBit, const Bit& rhsBit) { return lhsBit == rhsBit ? lhsBit : 0; } },
};

void leftShift(BinaryData& data, size_t loops = 1) noexcept
{
    if(data.empty())
    {
        data.push_back(1);
        --loops;
    }
    for(size_t i{0}; i < loops; ++i)
    {
        if(data.size() == std::numeric_limits<size_t>::max()) break;
        data.push_back(0);
    }
}

void rightShift(BinaryData& data, size_t& units, size_t loops)
{
    for(size_t i{0}; i < loops; ++i)
    {
        Bit& bit = data.back();
        if(data.size() == 1)
        {
            if(bit == 1)
            {
                bit = 0;
                --units;
                break;
            }
        }
        else
        {
            if(bit == 1) --units;
            data.pop_back();
        }
    }
}

void rightShift(BinaryData& data, size_t loops = 1)
{
    for(size_t i{0}; i < loops; ++i)
    {
        Bit& bit = data.back();
        if(data.size() == 1)
        {
            if(bit == 1)
            {
                bit = 0;
                break;
            }
        }
        else data.pop_back();
    }
}

void removeInsignificantBits(BinaryData& number)
{
    while(true)
    {
        auto it = begin(number);
        if(*it == 1 || number.size() == 1) break;
        if(*it == 0) number.pop_front();
    }
}

void Increment(BinaryData& data, size_t& units)
{
    for(auto it = data.rbegin(); it != data.rend(); ++it)
    {
        if(*it == 0)
        {
            *it = 1;
            ++units;
            break;
        }
        *it = 0;
        --units;
    }

    if(*data.begin() == 0)
    {
        data.push_front(1);
        ++units;
    }
}

void Decrement(BinaryData& data, size_t& units)
{
    for(auto it = data.rbegin(); it != data.rend(); ++it)
    {
        if(*it == 1)
        {
            *it = 0;
            --units;
            break;
        }
        *it = 1;
        ++units;
    }

    removeInsignificantBits(data);
}

template<class Iterator>
constexpr void isRandomAccessIterator()
{
    static_assert (std::is_same_v<typename std::iterator_traits<Iterator>::iterator_category,
                std::random_access_iterator_tag>, "Iterator should be random access iterator");
}

struct Less
{
    bool operator()(const BinaryData& lhs, const BinaryData& rhs)
    {
        // is lhs less than rhs
        return lhs.size() < rhs.size() ? true :
               lhs.size() > rhs.size() ? false :
               // lhs.size() == rhs.size()
               std::lexicographical_compare(std::execution::par_unseq, lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    template<class Iterator>
    bool operator()(Iterator lhs_begin, Iterator lhs_end, const BinaryData& rhs)
    {
        isRandomAccessIterator<Iterator>();
        if(size_t dist = std::distance(lhs_begin, lhs_end); dist < rhs.size()) return true;
        else if(dist > rhs.size()) return false;
        // dist == rhs.size()
        return std::lexicographical_compare(std::execution::par_unseq, lhs_begin, lhs_end, rhs.cbegin(), rhs.cend());
    }

    template<class Iterator>
    bool operator()(const BinaryData& rhs, Iterator lhs_begin, Iterator lhs_end)
    {
        isRandomAccessIterator<Iterator>();
        if(size_t dist = std::distance(lhs_begin, lhs_end); dist < rhs.size()) return false;
        else if(dist > rhs.size()) return true;
        // dist == rhs.size()
        return std::lexicographical_compare(std::execution::par_unseq, rhs.cbegin(), rhs.cend(), lhs_begin, lhs_end);
    }
} less;

struct Greater
{
    bool operator()(const BinaryData& lhs, const BinaryData& rhs)
    {
        return less(rhs, lhs);
    }

    template<class Iterator>
    bool operator()(Iterator lhs_begin, Iterator lhs_end, const BinaryData& rhs)
    {
        return less(rhs, lhs_begin, lhs_end);
    }
} greater;

struct Equal
{
    bool operator()(const BinaryData& lhs, const BinaryData& rhs)
    {
        if( lhs.size() != rhs.size() ) return false;
        // lhs.size() == rhs.size()
        return std::equal(std::execution::par_unseq, lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<class Iterator>
    bool operator()(Iterator lhs_begin, Iterator lhs_end, const BinaryData& rhs)
    {
        isRandomAccessIterator<Iterator>();
        if( std::distance(lhs_begin, lhs_end) != rhs.size() ) return false;
        // dist == rhs.size()
        return std::equal(std::execution::par_unseq, lhs_begin, lhs_end, rhs.begin(), rhs.end());
    }
} equal;

struct LessOrEqual
{
    bool operator()(const BinaryData& lhs, const BinaryData& rhs)
    {
        return less(lhs, rhs) ? true : equal(lhs, rhs);
    }

    template<class Iterator>
    bool operator()(const BinaryData& rhs, Iterator lhs_begin, Iterator lhs_end)
    {
        isRandomAccessIterator<Iterator>();
        return less(rhs, lhs_begin, lhs_end) ? true : equal(lhs_begin, lhs_end, rhs);
    }

} lessOrEqual;

struct GreatOrEqual
{
    bool operator()(const BinaryData& lhs, const BinaryData& rhs)
    {
        return greater(lhs, rhs) ? true : equal(lhs, rhs);
    }
} greatOrEqual;

struct Subtraction
{
    BinaryReturnType operator()(const BinaryData& minuend, const BinaryData& subtrahend)
    {
        assert(lessOrEqual(subtrahend, minuend));
        m_units = 0;
        auto [result, it, isLoad] = proccess(subtrahend.crbegin(), subtrahend.crend(), minuend.crbegin());
        addRest(result, it, minuend.crend(), isLoad);
        removeInsignificantBits(result);
        return {result, m_units};
    }

private:
    size_t m_units;

    template<class Iterator>
    void addRest(BinaryData& number, Iterator startIt, Iterator endIt, bool isLoan)
    {
        if(isLoan)
        {
            for(; startIt != endIt; ++startIt)
            {
                if(*startIt == 1)
                {
                    number.push_front(0);
                    ++startIt;
                    isLoan = false;
                    break;
                }

                number.push_front(1);
                ++m_units;
            }
        }

        for(;startIt != endIt; ++startIt)
        {
            if(*startIt == 1) ++m_units;
            number.push_front(*startIt);
        }

        assert(startIt == endIt);
        removeInsignificantBits(number);
    }

    template<class Iterator>
    ReturnType<Iterator> proccess(Iterator subtrahendStart, Iterator subtrahendEnd, Iterator minuendIt)
    {
        BinaryData result;
        bool isLoan = false;
        for(; subtrahendStart != subtrahendEnd; ++subtrahendStart, ++minuendIt)
        {
            if(*minuendIt > *subtrahendStart) // 1 0
            {
                if(isLoan)
                {
                    isLoan = false;
                    result.push_front(0);
                }
                else
                {
                    ++m_units;
                    result.push_front(1);
                }
            }
            else if(*minuendIt < *subtrahendStart) // 0 1
            {
                if(!isLoan)
                {
                    ++m_units;
                    result.push_front(1);
                    isLoan = true;
                }
                else result.push_front(0);
            }
            else // 1 1 or 0 0
            {
                if(isLoan)
                {
                    ++m_units;
                    result.push_front(1);
                }
                else result.push_front(0);
            }
        }

        return {result, minuendIt, isLoan};
    }

} subtraction;

struct Addition
{
    BinaryReturnType operator()(const BinaryData& lhs, const BinaryData& rhs)
    {
        m_units = 0;
        if(lhs.size() <= rhs.size())
        {
            auto [result, it, isTransfer] = proccess(lhs.crbegin(), lhs.crend(), rhs.crbegin());
            addRest(result, it, rhs.crend(), isTransfer);
            return {result, m_units};
        } else
        {
            auto [result, it, isTransfer] = proccess(rhs.crbegin(), rhs.crend(), lhs.crbegin());
            addRest(result, it, lhs.crend(), isTransfer);
            return {result, m_units};
        }
    }

private:

    size_t m_units;

    template<class Iterator>
    void addRest(BinaryData& number, Iterator startIt, Iterator endIt, bool isTransfer)
    {
        if(isTransfer)
        {
            for(; startIt != endIt; ++startIt)
            {
                if(*startIt == 0)
                {
                    number.push_front(1);
                    ++m_units;
                    isTransfer = false;
                    ++startIt;
                    break;
                }

                number.push_front(0);
            }
        }

        for(;startIt != endIt; ++startIt)
        {
            if(*startIt == 1) ++m_units;
            number.push_front(*startIt);
        }

        if(isTransfer)
        {
            number.push_front(1);
            ++m_units;
            isTransfer = false;
        }
    }

    template<class Iterator>
    ReturnType<Iterator> proccess(Iterator lhsIt, Iterator endIt, Iterator rhsIt)
    {
        BinaryData result;
        bool isTransfer = false;
        for(; lhsIt != endIt; ++rhsIt, ++lhsIt)
        {
            if(*lhsIt == 1 && *rhsIt == 1)
            {
                if(isTransfer)
                {
                    ++m_units;
                    result.push_front(1);
                }
                else
                {
                    isTransfer = true;
                    result.push_front(0);
                }
            } else if(*lhsIt == 0 && *rhsIt == 0)
            {
                if (isTransfer)
                {
                    result.push_front(1);
                    ++m_units;
                }
                else result.push_front(0);

                isTransfer = false;
            }
            else if(*lhsIt != *rhsIt)
            {
                if(isTransfer)
                    result.push_front(0);
                else
                {
                    result.push_front(1);
                    ++m_units;
                }
            }
        }

        return {result, rhsIt, isTransfer};
    }

} addition;

struct Division
{
    enum class Mode
    {
        Div,
        Mod
    };

    BinaryReturnType operator()(BinaryData dividend, BinaryData divisor, Mode mode)
    {
        BinaryData result;
        BinaryData tmp = divisor;
        size_t units = 0;

        while(greatOrEqual(dividend, divisor))
        {
            size_t diff = dividend.size() - divisor.size();
            auto offsetIt = std::next(dividend.cbegin(), divisor.size());
            size_t offset = lessOrEqual(divisor, dividend.cbegin(), offsetIt) ? diff : --diff;

            if(mode == Mode::Div)
            {
                std::tie(result, units) = addition(result, PrepareShiftedNumber(offset) );
                std::tie(dividend, std::ignore) = subtraction(dividend, PrepareShiftedNumber(tmp, offset));
            }
            else // mode == Mode::Mod
                std::tie(dividend, units) = subtraction(dividend, PrepareShiftedNumber(tmp, offset));
        }

        return mode == Mode::Div ? std::make_tuple(result, units)
                                 : std::make_tuple(dividend, units);
    }

private:

    BinaryData PrepareShiftedNumber(const BinaryData& data, size_t offset)
    {
        BinaryData result = data;
        leftShift(result, offset);
        return result;
    }

    BinaryData PrepareShiftedNumber(size_t offset)
    {
        BinaryData result{1};
        leftShift(result, offset);
        return result;
    }

} division;

struct Multiplication
{
    BinaryReturnType operator()(const BinaryData& multiplicand, const BinaryData& multiplier)
    {
        BinaryData result;
        BinaryData tmp = multiplier;
        auto changeSearchedBit = [](const Bit& bit) -> Bit { return bit == 1 ? 0 : 1; };
        Bit searchedBit = changeSearchedBit(multiplicand.back());
        size_t dist = 0, offset = dist;
        bool isFirstIter{false};

        for(auto it = multiplicand.crbegin(); it != multiplicand.crend();)
        {
            it = find_first_of(it, multiplicand.crend(), searchedBit);
            dist = std::distance(std::next(multiplicand.crbegin(), offset), it);
            offset += dist;
            leftShift(tmp, !isFirstIter ? dist : --dist);
            isFirstIter = true;
            searchedBit == 1 ?  ZeroToOne(result, tmp) : OneToZero(result, tmp, dist);
            searchedBit = changeSearchedBit(searchedBit);
        }

        return {result, m_units};
    }

private:

    size_t m_units;

    template<class Iterator>
    Iterator find_first_of(Iterator start, Iterator end, const Bit& bit) const
    {
        for(; start != end; ++start)
            if(*start == bit)
                return start;

        return end;
    }

    BinaryData PrepareShiftedNumber(const BinaryData& data, size_t offset)
    {
        BinaryData result = data;
        rightShift(result, offset);
        return result;
    }

    // 1 0
    void OneToZero(BinaryData& result, BinaryData& tmp, const size_t& offset)
    {
        std::tie(result, m_units) = addition(result, tmp);
        std::tie(result, m_units) = subtraction(result, PrepareShiftedNumber(tmp, offset));
        leftShift(tmp);
    }

    // 0 1
    void ZeroToOne(BinaryData& result, BinaryData& tmp)
    {
        std::tie(result, m_units) = addition(result, tmp);
        leftShift(tmp);
    }

} multiplication;

} // namesapce Operation

namespace Converter
{

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

        std::string result(std::max(static_cast<size_t>(1), tempResult.size()), '0');
        std::move(tempResult.begin(), tempResult.end(), result.begin());
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

public:
    static std::string ToBinary(const BinaryData& binary, Sign sign)
    {
        std::string result;

        if(sign == Sign::Positive)
        {
            std::transform(binary.cbegin(), binary.cend(), std::back_inserter(result), [](const Bit& bit) -> char
            { return toChar(bit); });
            return result;
        }
        else
        {
            bool isIncremented{false};
            std::transform(binary.crbegin(), binary.crend(), std::back_inserter(result), [&isIncremented](const Bit& bit) -> char
            {
                Bit newBit = bit == 1 ? 0 : 1;
                if(isIncremented == false && newBit == 0)
                {
                    isIncremented = true;
                    return '1';
                }
                if(isIncremented == false && newBit == 1)
                    return '0';

                if(isIncremented == true)
                    return toChar(newBit);
            });

            std::reverse(result.begin(), result.end());
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

        std::string result(std::max(static_cast<size_t>(1), tempResult.size()), '0');
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

        offset = number.find_first_not_of("0", offset);
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
        if(number == "-0" || number == "0") return State();
        BinaryData binary;
        size_t bits{0};
        size_t offset{0};
        Sign sign =  Sign::Positive;

        if(number.front() == '-')
        {
            sign = Sign::Negative;
            offset = 1;
        }

        while(offset != number.size())
        {
            Bit bit = getRemainder(number, offset);
            if(bit == 1) ++bits;
            binary.push_front(bit);
        }

        return State(binary, bits, sign);
    }

    template<class T>
    static State fromDecimal(T number)
    {
        BinaryData binary;
        size_t bits{0};
        Sign sign = Sign::Positive;
        if(number < 0)
        {
            number = -number;
            sign = Sign::Negative;
        }
        if(number == 0) return State();
        while(number > 0)
        {
            if(number&1) ++bits;
            binary.push_front(number&1);
            number /= 2;
        }

        return State(binary, bits, sign);
    }

public:

    template<class T>
    static State convert(T&& value)
    {
        return validator(value) ? fromDecimal(value) : State();
    }

};


} // namespace Converter


} // namespace



namespace BigInt
{

class BigInt final
{

    template<class T, class Callback>
    static BinaryReturnType process (const T& smaller, const T& bigger, Callback&& callback)
    {
        size_t bits{0};
        BinaryData result(bigger.size());
        std::transform(smaller.crbegin(), smaller.crend(), bigger.crbegin(), result.rbegin(), [&](const Bit& lhs, const Bit& rhs)
        {
            Bit bit = callback(lhs, rhs);
            if(bit == 1) ++bits;
            return bit;
        });

        size_t size = std::distance(smaller.crbegin(), smaller.crend());
        std::transform(std::next(bigger.crbegin(), size), bigger.crend(), std::next(result.rbegin(), size), [&](const Bit& bit)
        {
            if(bit == 1) ++bits;
            return bit;
        });

        Operation::removeInsignificantBits(result);
        return {result, bits};
    }

    template<class T, class Callback>
    static BinaryReturnType conditionForProcess (const T& lhs, const T& rhs, Callback&& callback)
    {
        return lhs.bit() <= rhs.bit() ?
                        process(lhs.Number(), rhs.Number(), callback) :
                        process(rhs.Number(), lhs.Number(), callback);
    }

    template<class Callback>
    static BigInt Transform(const BigInt& lhs, const BigInt& rhs, Callback&& callback)
    {
        auto[result, bits] = conditionForProcess(lhs.isNegative() ? ~lhs : lhs,
                                                 rhs.isNegative() ? ~rhs : rhs, callback);
        return lhs.sign() == rhs.sign() ? BigInt(result, bits) : ~(BigInt(result, bits));
    }

    State m_state;

    BigInt(const BinaryData& number, size_t bits = 0, Sign sign = Sign::Positive):
        m_state(number, bits, sign)
    {}

    BigInt(const State& state):
        m_state{state}
    {}

public:
    BigInt():
        m_state{State()}
    {}

    BigInt(const std::string value) :
        m_state{ Converter::ToBinary::convert( value ) }
    {}

    template<class T, class = typename std::enable_if_t<is_integer<T>()>>
    BigInt(T value)
        : m_state{ Converter::ToBinary::convert( value ) }
    {}

    inline const BinaryData& Number() const { return m_state.number; }
    inline size_t count() const { return m_state.bitSet; }
    inline size_t bit() const { return m_state.number.size(); }
    inline Sign sign() const { return m_state.sign; }
    inline bool is2Pow() const { return m_state.bitSet == 1; }
    bool isPrime() const
    {
        bool notPrime = bit() == 1 && (isZero() || isUnit()); // 0,1
        bool prime = !notPrime && bit() == 2 &&
                ( Operation::equal(BinaryData({1,0}), Number()) || Operation::equal(BinaryData({1,1}), Number())) ;
        if(notPrime) return false;
        if(prime) return true;

        if ( ( (pow(*this, 2)) % BigInt(BinaryData({1,1,0,0,0}), 2) ).isUnit())
            return true;

        return false;
    }
    inline bool isPositive() const { return m_state.sign == Sign::Positive; }
    inline bool isNegative() const { return !isPositive(); }
    inline bool isEven() const { return *Number().rbegin() == 0; }
    inline bool isOdd() const { return !isEven(); }
    inline bool isZero() const { return bit() == 1 && *Number().begin() == 0; }
    inline bool isUnit() const { return bit() == 1 && *Number().begin() == 1; }
    inline void MakePositive() { m_state.sign = Sign::Positive; }
    inline void MakeNegative() { m_state.sign = Sign::Negative; }
    inline std::string toBinary() const { return Converter::FromBinary::ToBinary(m_state.number, m_state.sign); }
    inline std::string toOctal() const
    {
        return isNegative() ? '-' + Converter::FromBinary::ToOctal(m_state.number)
                            : Converter::FromBinary::ToOctal(m_state.number);
    }
    inline std::string toDecimal() const
    {
        return isNegative() ? '-' + Converter::FromBinary::ToDecimal(m_state.number)
                            : Converter::FromBinary::ToDecimal(m_state.number);
    }
    inline std::string toHex() const
    {
        return  isNegative() ? '-' + Converter::FromBinary::ToHex(m_state.number)
                             : Converter::FromBinary::ToHex(m_state.number);
    }

    friend BigInt operator + (const BigInt& lhs, const BigInt& rhs)
    {
        if( (lhs.isPositive() && rhs.isPositive()) ||
                (lhs.isNegative() && rhs.isNegative()) )
        {
            auto [result, bits] = Operation::addition(lhs.Number(), rhs.Number());
            return BigInt(result, bits, Sign::Positive);
        }
        if(lhs.isNegative())
        {
            if(Operation::less(lhs.Number(), rhs.Number()))
            {
                auto [result, bits] = Operation::subtraction(lhs.Number(), rhs.Number());
                return BigInt(result, bits, Sign::Positive);
            }
            else
            {
                auto [result, bits] = Operation::subtraction(rhs.Number(), lhs.Number());
                return BigInt(result, bits, Sign::Negative);
            }
        } else if(rhs.isNegative())
        {
            if(Operation::less(rhs.Number(), lhs.Number()))
            {
                auto [result, bits] = Operation::subtraction(lhs.Number(), rhs.Number());
                return BigInt(result, bits, Sign::Positive);
            }
            else
            {
                auto [result, bits] = Operation::subtraction(rhs.Number(), lhs.Number());
                return BigInt(result, bits, Sign::Negative);
            }
        }

        return BigInt();
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator + (const BigInt& lhs, T rhs)
    {
        return operator+(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator + (T lhs, const BigInt& rhs)
    {
        return operator+(BigInt(lhs), rhs);
    }

    BigInt& operator += (const BigInt& other)
    {
        *this = operator+(*this, other);
        return *this;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    BigInt& operator += (T other)
    {
        *this = operator+(*this, BigInt(other));
        return *this;
    }

    friend BigInt operator - (const BigInt& lhs, const BigInt& rhs)
    {
        if(lhs.isPositive() && rhs.isNegative())
        {
            auto [result, bits] = Operation::addition(lhs.Number(), rhs.Number());
            return BigInt(result, bits, Sign::Positive);
        }
        else if(lhs.isNegative() && rhs.isPositive())
        {
            auto [result, bits] = Operation::addition(lhs.Number(), rhs.Number());
            return BigInt(result, bits, Sign::Negative);
        }

        bool isBigger = Operation::greater(lhs.Number(), rhs.Number());
        Sign sign = (lhs.isNegative() && rhs.isNegative()) ?
                    (isBigger ? Sign::Negative : Sign::Positive) :
                    (isBigger ? Sign::Positive : Sign::Negative);
        auto [result, bits] = isBigger ?
                Operation::subtraction(lhs.Number(), rhs.Number()) :
                Operation::subtraction(rhs.Number(), lhs.Number());

        return BigInt(result, bits, sign);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator - (const BigInt& lhs, T rhs)
    {
        return operator-(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator - (T lhs, const BigInt& rhs)
    {
        return operator-(BigInt(lhs), rhs);
    }

    BigInt& operator -= (const BigInt& other)
    {
        *this = operator-(*this, other);
        return *this;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    BigInt& operator -= (T other)
    {
        *this = operator-(*this, BigInt(toBinary(other)));
        return *this;
    }

    friend BigInt operator * (const BigInt& lhs, const BigInt& rhs)
    {
        if(lhs.isZero() || rhs.isZero()) return BigInt();
        Sign sign = lhs.sign() == rhs.sign() ? Sign::Positive : Sign::Negative;
        if(lhs.isUnit()) return rhs;
        if(rhs.isUnit()) return lhs;
        if(lhs.is2Pow()) return rhs << lhs.bit()-1;
        if(rhs.is2Pow()) return lhs << rhs.bit()-1;

        auto[result, bits] = Operation::multiplication(lhs.Number(), rhs.Number());
        return BigInt(result, bits, sign);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator * (const BigInt& lhs, T rhs)
    {
        return operator*(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator * (T lhs, const BigInt& rhs)
    {
        return operator*(BigInt(lhs), rhs);
    }

    BigInt& operator *= (const BigInt& other)
    {
        *this = operator*(*this, other);
        return *this;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    BigInt& operator *= (T other)
    {
        *this = operator*(*this, BigInt(toBinary(other)));
        return *this;
    }

    friend BigInt operator / (const BigInt& lhs, const BigInt& rhs)
    {
        if(rhs.isZero()) throw "Division by zero";
        if(lhs < rhs) return BigInt();
        if(rhs.isUnit()) return lhs;
        if(rhs.is2Pow()) return lhs >> (rhs.bit()-1);

        Sign sign = lhs.sign() == rhs.sign() ? Sign::Positive : Sign::Negative;
        auto [result, bits] = Operation::division(lhs.Number(), rhs.Number(), Operation::Division::Mode::Div);
        return BigInt(result, bits, sign);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator / (const BigInt& lhs, T rhs)
    {
        return operator/(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator / (T lhs, const BigInt& rhs)
    {
        return operator/(BigInt(lhs), rhs);
    }

    BigInt& operator /= (const BigInt& other)
    {
        *this = operator/(*this, other);
        return *this;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    BigInt& operator /= (T other)
    {
        *this = operator/(*this, BigInt(toBinary(other)));
        return *this;
    }

    friend BigInt operator % (const BigInt& lhs, const BigInt& rhs)
    {
        if(lhs < rhs) return lhs;
        Sign sign = lhs.isNegative() ? Sign::Negative : Sign::Positive;
        auto [result, bits] = Operation::division(lhs.Number(), rhs.Number(), Operation::Division::Mode::Mod);
        return BigInt(result, bits, sign);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator % (const BigInt& lhs, T rhs)
    {
        return operator%(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator % (T lhs, const BigInt& rhs)
    {
        return operator%(BigInt(lhs), rhs);
    }

    BigInt& operator %= (const BigInt& other)
    {
        *this = operator%(*this, other);
        return *this;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    BigInt& operator %= (T other)
    {
        *this = operator%(*this, BigInt(toBinary(other)));
        return *this;
    }

    friend bool operator < (const BigInt& lhs, const BigInt& rhs)
    {
        if(lhs.isPositive() && rhs.isNegative()) return false;
        if(lhs.isNegative() && rhs.isPositive()) return true;
        if(lhs.isNegative() && rhs.isNegative()) return Operation::greater(lhs.Number(), rhs.Number());
        return Operation::less(lhs.Number(), rhs.Number());
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator < (const BigInt& lhs, T rhs)
    {
        return operator <(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator < (T lhs, const BigInt& rhs)
    {
        return operator <(BigInt(lhs), rhs);
    }

    friend bool operator <= (const BigInt& lhs, const BigInt& rhs)
    {
        return Operation::lessOrEqual(lhs.Number(), rhs.Number());
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator <= (const BigInt& lhs, T rhs)
    {
        return operator <=(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator <= (T lhs, const BigInt& rhs)
    {
        return operator <=(BigInt(lhs), rhs);
    }

    friend bool operator > (const BigInt& lhs, const BigInt& rhs)
    {
        if(lhs.isPositive() && rhs.isNegative()) return true;
        if(lhs.isNegative() && rhs.isPositive()) return false;
        if(lhs.isNegative() && rhs.isNegative()) return Operation::less(lhs.Number(), rhs.Number());
        return Operation::greater(lhs.Number(), rhs.Number());
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator > (const BigInt& lhs, T rhs)
    {
        return operator >(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator > (T lhs, const BigInt& rhs)
    {
        return operator >(BigInt(lhs), rhs);
    }

    friend bool operator >= (const BigInt& lhs, const BigInt& rhs)
    {
        return Operation::greatOrEqual(lhs.Number(), rhs.Number());
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator >= (const BigInt& lhs, T rhs)
    {
        return operator >=(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator >= (T lhs, const BigInt& rhs)
    {
        return operator >=(BigInt(lhs), rhs);
    }

    friend bool operator == (const BigInt& lhs, const BigInt& rhs)
    {
        if(lhs.sign() != rhs.sign()) return false;
        return Operation::equal(lhs.Number(), rhs.Number());
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator == (const BigInt& lhs, T rhs)
    {
        return operator ==(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator == (T lhs, const BigInt& rhs)
    {
        return operator ==(BigInt(lhs), rhs);
    }

    friend bool operator != (const BigInt& lhs, const BigInt& rhs)
    {
        return !operator ==(lhs, rhs);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator != (const BigInt& lhs, T rhs)
    {
        return operator !=(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend bool operator != (T lhs, const BigInt& rhs)
    {
        return operator !=(BigInt(lhs), rhs);
    }

    friend BigInt operator ^ (const BigInt& lhs, const BigInt& rhs)
    {
        return Transform(lhs, rhs, Operation::predicates['^']);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator ^ (const BigInt& lhs, T rhs)
    {
        return operator^(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator ^ (T lhs, const BigInt& rhs)
    {
        return operator^(BigInt(lhs), rhs);
    }

    BigInt& operator ^= (const BigInt& other)
    {
        *this = Transform(*this, other, Operation::predicates['^']);
        return *this;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    BigInt& operator ^= (T other)
    {
        return operator^=(BigInt(toBinary(other)));
    }

    friend BigInt operator | (const BigInt& lhs, const BigInt& rhs)
    {
        return Transform(lhs, rhs, Operation::predicates['|']);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator | (const BigInt& lhs, T rhs)
    {
        return operator|(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator | (T lhs, const BigInt& rhs)
    {
        return operator|(BigInt(lhs), rhs);
    }

    BigInt& operator |= (const BigInt& other)
    {
        *this = Transform(*this, other, Operation::predicates['|']);
        return *this;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    BigInt& operator |= (T other)
    {
        return operator|=(BigInt(toBinary(other)));
    }

    friend BigInt operator & (const BigInt& lhs, const BigInt& rhs)
    {
        return Transform(lhs, rhs, Operation::predicates['&']);
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator & (const BigInt& lhs, T rhs)
    {
        return operator&(lhs, BigInt(rhs));
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    friend BigInt operator & (T lhs, const BigInt& rhs)
    {
        return operator&(BigInt(lhs), rhs);
    }

    BigInt& operator &= (const BigInt& other)
    {
        *this = Transform(*this, other, Operation::predicates['&']);
        return *this;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>>>
    BigInt& operator &= (T other)
    {
        return operator&=(BigInt(toBinary(other)));
    }

    friend BigInt operator ~(const BigInt& number)
    {
        if(number.isZero())
            return BigInt(BinaryData{1}, 1, Sign::Negative);

        if(number.isPositive())
        {
            BinaryData data = number.Number();
            size_t bits = number.count();
            Operation::Increment(data, bits);
            return BigInt(data, bits, Sign::Negative);
        }
        else
        {
            BinaryData data = number.Number();
            size_t bits = number.count();
            Operation::Decrement(data, bits);
            return BigInt(data, bits, Sign::Positive);
        }

        return BigInt();
    }

    friend BigInt operator -(const BigInt& number)
    {
        return BigInt(number.Number(), number.count(), number.isPositive() ? Sign::Negative : Sign::Negative);
    }

    BigInt operator !()
    {
        return isZero() ? BigInt(BinaryData{1}, 1, Sign::Positive) : BigInt();
    }

    friend BigInt operator << (const BigInt& lhs, const size_t offset)
    {
        BinaryData newResult = lhs.Number();
        Operation::leftShift(newResult, offset);
        return BigInt(newResult, lhs.count(), lhs.sign());
    }

    BigInt& operator <<= (const size_t offset)
    {
        Operation::leftShift(m_state.number, offset);
        return *this;
    }

    friend BigInt operator >> (const BigInt& lhs, const size_t offset)
    {
        BinaryData newResult = lhs.Number();
        size_t units = lhs.count();
        Operation::rightShift(newResult, units, offset);
        return BigInt(newResult, units, lhs.sign());
    }

    BigInt& operator >>= (const size_t offset)
    {
        Operation::rightShift(m_state.number, m_state.bitSet, offset);
        return *this;
    }

    friend std::ostream& operator << (std::ostream& os, const BigInt& number)
    {
        os << number.toDecimal();
        return os;
    }

    friend std::istream& operator >> (std::istream& is, BigInt& number)
    {
        std::string str;
        is >> str;
        number = str;
        return is;
    }

    BigInt& operator++()
    {
           isNegative() ? Operation::Decrement(m_state.number, m_state.bitSet)
                      : Operation::Increment(m_state.number, m_state.bitSet);
        return *this;
    }

    BigInt& operator--()
    {
        if(isZero())
            *this = BigInt(BinaryData{1}, 1, Sign::Negative);
        else if(isPositive())
            Operation::Decrement(m_state.number, m_state.bitSet);
        else if(isNegative())
            Operation::Increment(m_state.number, m_state.bitSet);

        return *this;
    }

    BigInt operator++(int)
    {
        auto tmp = *this;
        isNegative() ? Operation::Decrement(m_state.number, m_state.bitSet)
                     : Operation::Increment(m_state.number, m_state.bitSet);
        return tmp;
    }

    BigInt operator--(int)
    {
        auto tmp = *this;
        if(isZero())
            *this = BigInt(BinaryData{1}, 1, Sign::Negative);
        else if(isPositive())
            Operation::Decrement(m_state.number, m_state.bitSet);
        else if(isNegative())
            Operation::Increment(m_state.number, m_state.bitSet);

        return tmp;
    }

    template<class T, class = typename std::enable_if_t<is_integer<T>()>>
    explicit operator T()
    {
        if(isNegative() && is_unsigned<T>()) return 0;

        const short byteLenght = 8;
        const unsigned short lenght = sizeof(T) * byteLenght;
        const bool condition = (isNegative() ? is2Pow() : count() == bit());

        if(lenght > bit() || (lenght == bit() && condition ) )
            return isNegative() ? -1*Converter::FromBinary::toInt<T>(Number())
                                : Converter::FromBinary::toInt<T>(Number());

        return 0;
    }

    static BigInt abs(const BigInt& number)
    {
        return number.isNegative() ? BigInt(number.Number(), number.bit(), Sign::Positive)
                                   : number;
    }

    static BigInt pow(const BigInt& number, size_t pow)
    {
        if(pow == 0) return BigInt(1);
        if(pow == 1) return number;

        static auto countBits = [](const size_t& number) -> size_t
        {
            return static_cast<size_t>(log2(number));
        };

        BigInt result(1);
        BigInt tempResult = number;

        while(pow)
        {
            if(pow == 1)
            {
                result *= number;
                break;
            }
            size_t count = countBits(pow);
            for(size_t i{0}; i < count; ++i)
                tempResult *= tempResult;

            pow -= 1 << count;
            result *= tempResult;
            tempResult = number;
        }

        return result;
    }

    static std::vector<BigInt> factorize(BigInt number)
    {
        std::vector<BigInt> factors;
        BigInt divisor(2);
        while(divisor*divisor <= number)
        {
            if((number%divisor).isZero())
            {
                factors.push_back(divisor);
                number /= divisor;
            }
            else if(divisor == 2) divisor = BigInt(3);
            else divisor += 2;
        }

        factors.push_back(number);

        return factors;
    }

    static BigInt fibonacci(std::size_t nElem)
    {
        BigInt a(1), b(1), c(1), rd(1);
        BigInt ta, tb, rc, tc, d;

        while(nElem)
        {
            if(nElem&1) // isOdd
            {
                tc = rc;
                rc = rc*a + rd*c;
                rd = tc*b + rd*d;
            }

            ta = a;
            tb = b;
            tc = c;
            a = a*a + b*c;
            b = ta*b + b*d;
            c = c*ta + d*c;
            d = tc*tb+ d*d;

            nElem /= 2;
        }

        return rc;
    }

    static BigInt factorial(std::size_t nElem)
    {
        if(nElem == 1 || nElem == 0)
            return BigInt(1);

        bool handleOdd {false};
        size_t uptoNumber {nElem};

        if((nElem & 1) == 1)
        {
            --uptoNumber;
            handleOdd = true;
        }

        BigInt nextSum(uptoNumber);
        BigInt nextMulti(uptoNumber);
        BigInt factorial(1);

        while(nextSum >= 2)
        {
            factorial *= nextMulti;
            nextSum -= 2;
            nextMulti += nextSum;
        }

        if (handleOdd) factorial *= nElem;

        return factorial;
    }

    static BigInt gcd(const BigInt& u, const BigInt& v)
    {
        if (u == v) return u;
        if (u.isZero()) return v;
        if (v.isZero()) return u;

        if (u.isEven())
        {
            if (v.isOdd()) return gcd(u/2, v);
            else return 2 * gcd(u/2, v/2);
        }
        else
        {
            if (v.isEven()) return gcd(u, v/2);
            if (u > v) return gcd((u - v)/2, v);
            else return gcd((v - u)/2, u);
        }
    }

    static BigInt lcm(const BigInt& a, const BigInt& b)
    {
        return (a*b) / gcd(a,b);
    }

    static BigInt isqrt(BigInt number)
    {
        if(number.isNegative() || number.isZero() || number.isUnit()) return BigInt();
        BigInt result;
        const short roundingToLowPow2 = 2;
        BigInt bit = 1 << (number.bit()-roundingToLowPow2);

        while (!bit.isZero())
        {
            if(auto add = result + bit; number >= add)
            {
                number -= add;
                result = (result>>1) + bit;
            }
            else result >>= 1;

            bit >>= 2;
        }

        return result;
    }

    static BigInt ilog2(const BigInt& number)
    {
        return BigInt(number.bit()-1);
    }

    static BigInt ilog10(const BigInt& number)
    {
        const short base = 10;
        if(number < base) return BigInt();
        return number.toDecimal().size()-1;
    }

    static bool isPow(BigInt number, const BigInt& pow)
    {
        while(!number.isUnit())
        {
            if(!((number % pow).isZero())) break;
            number /= pow;
        }

        return number.isUnit();
    }
};

} // namespace BigInt
