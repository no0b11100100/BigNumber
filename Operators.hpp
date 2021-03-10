#pragma once
#include <deque>
#include <execution>
#include <algorithm>
#include <climits>
#include <cassert>
#include <iostream>
#include <thread>
#include <string>

using Bit = bool;
using BinaryData = std::deque<Bit>;
using BinaryReturnType = std::tuple<BinaryData, size_t>;

template<class Iterator>
using ReturnType = std::tuple<BinaryData, Iterator, bool>;

void leftShift(BinaryData& data, const size_t loops = 1) noexcept
{
    for(size_t i{0}; i < loops; ++i)
    {
        if(data.size() == std::numeric_limits<size_t>::max()) break;
//        if(data.empty()) data.push_back(1);
        else data.push_back(0);
    }
}

void rightShift(BinaryData& data, const size_t loops = 1) noexcept
{
    for(size_t i{0}; i < loops; ++i)
    {
        if(data.size() == 1)
        {
            if(auto it = data.begin(); *it == 1) *it = 0;
            break;
        }
        data.pop_front();
    }
}

void rightShift(BinaryData& data, size_t& bits, const size_t loops = 1) noexcept
{
    for(size_t i{0}; i < loops; ++i)
    {
        auto it = data.begin();
        if(data.size() == 1)
        {
            if(*it == 1)
            {
                *it = 0;
                --bits;
            }
            break;
        }

        if(*it == 1) --bits;
        data.pop_front();
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

struct Subtraction
{
    BinaryReturnType operator()(const BinaryData& minuend, const BinaryData& subtrahend)
    {
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
    // TODO: need tests
    BinaryReturnType operator()(BinaryData dividend, BinaryData divisor, Mode mode)
    {
        assert(greater( dividend, divisor ) == true);
        BinaryData result;
        BinaryData tmp = divisor;
        BinaryData shiftedData;
        size_t units = 0;

        while(less(divisor, dividend))
        {
            size_t diff = dividend.size() - divisor.size();
            auto offsetIt = std::next(dividend.cbegin(), divisor.size());

            if( greater(dividend.cbegin(), offsetIt, divisor) )
                PrepareNumbers(tmp, shiftedData, diff, units);
            else
                PrepareNumbers(tmp, shiftedData, --diff, units);

            if(mode == Mode::Div)
                std::tie(result, units) = addition(result, shiftedData);

            std::tie(dividend, std::ignore) = subtraction(dividend, tmp);
        }

        return mode == Mode::Div ? std::make_tuple(result, units)
                                 : std::make_tuple(dividend, units);
    }

private:

    static void prepareShiftedData(BinaryData& shiftedData, size_t count, size_t& units)
    {
        size_t size = shiftedData.size();
        if(size < count)
            leftShift(shiftedData, count-size);
        else
            rightShift(shiftedData, units, size-count);
    }

    static void PrepareNumbers(BinaryData& result, BinaryData& shiftedData, size_t count, size_t& units)
    {
        std::thread prepareShiftedDataThread(prepareShiftedData, std::ref(shiftedData), count, std::ref(units));
        leftShift(result, count);
        prepareShiftedDataThread.join();
    }

} division;

struct Multiplication
{
    BinaryReturnType operator()(const BinaryData& lhs, const BinaryData& rhs)
    {
        return proccess(lhs.crbegin(), lhs.crend(), rhs);
    }

private:

    size_t m_units;

    void handleNextZero(BinaryData& result, BinaryData& tmp, const BinaryData& other, size_t& offset)
    {
        if(offset > 1)
        {
            std::tie(result, m_units) = addition(result, tmp);
            if(offset > 1) std::tie(result, m_units) = subtraction(result, other);
            offset = 0;
        }
    }

    template<class Iterator>
    BinaryReturnType proccess(Iterator startIt, Iterator endIt, const BinaryData& other)
    {
        BinaryData result;
        BinaryData tmp;
        size_t offset = 0;
        std::copy(std::execution::par_unseq, startIt, endIt, std::back_inserter(tmp));
        for(; startIt != endIt; ++startIt)
        {
            if(*startIt == 0) leftShift(tmp);
            if(*startIt == 1)
            {
                ++offset;
                if( *std::next(startIt) == 0 || std::next(startIt) == endIt )
                    handleNextZero(result, tmp, other, offset);

                leftShift(tmp);
            }
        }

        return {result, m_units};
    }

} multiplication;

struct Square
{
    BinaryData operator()(BinaryData number)
    {
        assert(number.size() != 0);
        BinaryData tmp(number.size()-1);
        BinaryData result;
        result.push_front(1);

        while(!isZero(number))
        {
            if( auto[add, bits] = addition(result, tmp); less(number, add ) )
            {
                auto [sub, ignore] = subtraction(result, tmp);
                std::tie(number, std::ignore) =  subtraction(number, sub);
                rightShift(result);
                std::tie(result, std::ignore) = addition(result, tmp);
            }
            else rightShift(result);

            rightShift(tmp, 2);
        }

        return result;
    }

private:
    bool isZero(const BinaryData& number) const { return number.size() == 1 && *number.cbegin() == 0; }

} square;

struct MakeBinaryNegative
{
    size_t operator()(BinaryData& data)
    {
        size_t bits = inverseBits(data)+1;
        data.push_front(1);
        Increment(data, bits);
        return bits;
    }
private:

    size_t inverseBits(BinaryData& data)
    {
        size_t count;
        std::transform(data.begin(), data.end(), data.begin(), [&count](const Bit& bit) -> Bit
        {
            if(bit == 0) ++count;
            return !bit;
        });

        return count;
    };
} binaryNegative;

struct Predicate
{
    enum class Case
    {
        PositivePositive,
        NegativeNegative,
        PositiveNegative,
        NegativePositive
    };
private:
    bool isNegativeIncrement1{false};
    bool isNegativeIncrement2{false};

    using Callback = std::function<Bit(const Bit&, const Bit&)>;

    Bit differentSigns(const Bit& positiveBit, const Bit& negativeBit, Callback callback, bool& isInceremnt)
    {
        Bit newNegativeBit = !negativeBit;
        if(!isInceremnt && newNegativeBit == 0)
        {
            newNegativeBit = 1;
            isInceremnt = true;
        }

        return callback(positiveBit, newNegativeBit);
    }

    Bit NegativeNegative(const Bit& lhsBit, const Bit& rhsBit, Callback callback, bool& isInceremntLhs, bool& isInceremntRhs)
    {
        Bit newLhsBit {!lhsBit};
        Bit newRhsBit {!rhsBit};
        if(!isInceremntLhs && newLhsBit == 0)
        {
            newLhsBit = 1;
            isInceremntLhs = true;
        }

        if(!isInceremntRhs && newRhsBit == 0)
        {
            newRhsBit = 1;
            isInceremntRhs = true;
        }

        return callback(newLhsBit, newRhsBit);
    }

    Case m_case;
    Callback m_callback;
public:
    Bit operator()(const Bit& lhs, const Bit& rhs)
    {
        switch(m_case)
        {
        case Case::NegativePositive:
            return differentSigns(rhs, lhs, m_callback, isNegativeIncrement1);
        case Case::PositiveNegative:
            return differentSigns(lhs, rhs, m_callback, isNegativeIncrement1);
        case Case::NegativeNegative:
            return NegativeNegative(lhs, rhs, m_callback, isNegativeIncrement1, isNegativeIncrement2);
        default:
            return m_callback(lhs, rhs);
        }
    }

    Predicate(Case _case, Callback callback)
        : m_case{_case},
          m_callback{callback}
    {}

    Predicate() = delete;
    Predicate(const Predicate&) = delete;
    Predicate(Predicate&&) = delete;
    Predicate operator=(const Predicate&) = delete;
    Predicate& operator=(Predicate&&) = delete;
};
