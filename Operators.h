#pragma once
#include <deque>
#include <execution>
#include <algorithm>
#include <climits>
#include <cassert>
#include <iostream>

using Bit = bool;
using BinaryData = std::deque<Bit>;
using BinaryReturnType = std::tuple<BinaryData, size_t>;

template<class Iterator>
using ReturnType = std::tuple<BinaryData, Iterator, bool>;

void leftShift(BinaryData& data, Bit bit)
{
    if(data.size() == std::numeric_limits<size_t>::max()) return;
    data.push_back(bit);
}

BinaryData Shift(const BinaryData& data, size_t count, Bit bit)
{
    BinaryData result = data;
    for(size_t i {0}; i < count; ++i)
        result.push_back(bit);
    return result;
}

void rightShift(BinaryData& data)
{
    if(data.empty()) return;
    data.pop_front();
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
    BinaryData operator()(BinaryData dividend, BinaryData divisor)
    {
        assert(greater( dividend, divisor ) == true);
        BinaryData result;
        BinaryData tmp;

        // TODO: check is it possible to add 1 in cases < and ==
        while(less(divisor, dividend))
        {
            size_t diff = dividend.size() - divisor.size();
            auto offsetIt = std::next(dividend.begin(), divisor.size());
            if(greater( dividend.begin(), offsetIt, divisor ) )
            {
                tmp = Shift(divisor, diff, 1);
            }
            else if(equal(dividend.begin(), offsetIt, divisor))
            {
                tmp.clear();
                std::move(divisor.begin(), divisor.end(), std::back_inserter(tmp));
                std::for_each(offsetIt, dividend.end(), [&](Bit bit)
                {
                    leftShift(tmp, bit);
                });
            }
            else
            {
                tmp.clear();
                bool isFirstUnit = false;
                std::move(divisor.begin(), divisor.end(), std::back_inserter(tmp));

                if( std::find(std::execution::par_unseq, offsetIt, dividend.end(), 1) == dividend.end() )
                {
                    std::for_each(next(offsetIt, 1), dividend.end(), [&](Bit)
                    {
                        tmp.push_back(1);
                    });
                }
                else
                {
                    std::for_each(offsetIt, dividend.end(), [&](Bit bit)
                    {
                        if(bit == 1 && !isFirstUnit)
                        {
                            isFirstUnit = true;
                            tmp.push_back(0);
                        }
                        else if(bit == 0 && !isFirstUnit) tmp.push_back(0);
                        else tmp.push_back(1);
                    });
                }
            }

            dividend = minus(dividend, tmp);
        }

        for(auto v : dividend) std::cout << v << std::endl;

        return result;
    }

private:
    BinaryData Binary(int number)
    {
        BinaryData list_number;
        while(number > 0)
        {
            bool value = static_cast<bool>(number % 2);
            list_number.push_front(value);
            number >>= 1;
        }

        return list_number;
    }

    BinaryData minus(BinaryData a, BinaryData b){
           std::string s1;
           std::string s2;

           for(auto v : a) s1 += std::to_string(v);
           for(auto v : b) s2 += std::to_string(v);

           return Binary( std::strtoull(s1.c_str(), NULL, 2) - std::strtoull(s2.c_str(), NULL, 2)  );
       };

    BinaryData plus(BinaryData a, BinaryData b){
           std::string s1;
           std::string s2;

           for(auto v : a) s1 += std::to_string(v);
           for(auto v : b) s2 += std::to_string(v);

           return Binary( std::strtoull(s1.c_str(), NULL, 2) + std::strtoull(s2.c_str(), NULL, 2)  );
       };

} division;

struct Module
{
     BinaryReturnType operator()(const BinaryData& dividend, const BinaryData& divisor)
     {
         BinaryData result = dividend;
         BinaryData tmp;
         size_t units = 0;
         size_t diff = dividend.size() - divisor.size();
         auto offsetIt = std::next(dividend.begin(), divisor.size());

         while(less(divisor, dividend))
         {
             if( greater( dividend.begin(), offsetIt, divisor ) )
             {
                 tmp = Shift(divisor, diff, 0);
             }
             else
             {
                 tmp = Shift(divisor, --diff, 0);
             }

             std::tie(result, units) = subtraction(result, tmp);
         }

         return {result, units};
     }

} modulo;

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
            if(*startIt == 0) leftShift(tmp, 0);
            if(*startIt == 1)
            {
                ++offset;
                if( *std::next(startIt, 1) == 0 || std::next(startIt, 1) == endIt )
                    handleNextZero(result, tmp, other, offset);

                leftShift(tmp, 0);
            }
        }

        return {result, m_units};
    }

} multiplication;
