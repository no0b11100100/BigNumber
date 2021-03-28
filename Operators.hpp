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

void rightShift(BinaryData& data, const size_t loops = 1) noexcept
{
    for(size_t i{0}; i < loops; ++i)
    {
        if(data.size() == 1)
        {
            if(data.front() == 1) data.front() = 0;
            break;
        }
        data.pop_back();
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
        data.pop_back();
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
        for(size_t i = 0; i < offset; ++i)
            result.emplace_back(0);

        return result;
    }

    BinaryData PrepareShiftedNumber(size_t offset)
    {
        BinaryData result{1};
        for(size_t i = 0; i < offset; ++i)
            result.emplace_back(0);

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
        for(size_t i = 0; i < offset; ++i)
            rightShift(result);

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

    size_t m_units;

} multiplication;


struct Square
{
    BinaryData operator()(BinaryData number)
    {
        assert(number.size() != 0);
        BinaryData tmp(number.size()-1);
        BinaryData result(1);

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

            rightShift(tmp);
            rightShift(tmp);
        }

        return result;
    }

private:
    bool isZero(const BinaryData& number) const { return number.size() == 1 && number.front() == 0; }

} square;
