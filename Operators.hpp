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
        if(data.empty()) data.push_back(1);
        else data.push_back(0);
    }
}

void rightShift(BinaryData& data, const size_t loops = 1) noexcept
{
    // TODO: count how much 1 was removed
    for(size_t i{0}; i < loops; ++i)
    {
        if(data.size() == 1)
        {
            if(auto it = data.begin(); *it == 1)
                *it = 0;
            break;
        }
        data.pop_front();
    }
}

void Increment(BinaryData& data)
{
    for(auto it = data.rbegin(); it != data.rend(); ++it)
    {
        if(*it == 0)
        {
            *it = 1;
            break;
        }
        *it = 0;
    }

    if(*data.begin() == 0) data.push_front(1);
}

void Decrement(BinaryData& data)
{
    for(auto it = data.rbegin(); it != data.rend(); ++it)
    {
        if(*it == 1)
        {
            *it = 0;
            break;
        }
        *it = 1;
    }

    // TODO: remove 0
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

    void removeInsignificantBits(BinaryData& number)
    {
        while(true)
        {
            auto it = begin(number);
            if(*it == 1 || number.size() == 1) break;
            if(*it == 0) number.pop_front();
        }
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
                PrepareNumbers(tmp, shiftedData, diff);
            else
                PrepareNumbers(tmp, shiftedData, --diff);

            if(mode == Mode::Div)
                std::tie(result, units) = addition(result, shiftedData);

            std::tie(dividend, std::ignore) = subtraction(dividend, tmp);
        }

        return mode == Mode::Div ? std::make_tuple(result, units)
                                 : std::make_tuple(dividend, units);
    }

private:

    static void prepareShiftedData(BinaryData& shiftedData, size_t count)
    {
        size_t size = shiftedData.size();
        if(size < count)
            for(size_t i {size}; i < count; ++i)
                leftShift(shiftedData);
        else
            for(size_t i {size}; i < count; ++i)
                rightShift(shiftedData);
    }

    static void PrepareNumbers(BinaryData& result, BinaryData& shiftedData, size_t count)
    {
        std::thread prepareShiftedDataThread(prepareShiftedData, std::ref(shiftedData), count);

        for(size_t i {0}; i < count; ++i)
            leftShift(result);

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