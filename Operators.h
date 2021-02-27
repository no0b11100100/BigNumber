#pragma once
#include <deque>
#include <execution>
#include <algorithm>
#include <climits>
#include <cassert>
#include <iostream>

using Bit = bool;
using BinaryData = std::deque<Bit>;

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
constexpr void checkIteratorTag()
{
    static_assert (std::is_same_v<typename std::iterator_traits<Iterator>::iterator_category,
                std::random_access_iterator_tag>, "Iterator should be random access iterator");
}

struct Less
{
    bool operator()(const BinaryData& lhs, const BinaryData& rhs)
    {
        return std::lexicographical_compare(std::execution::par_unseq, lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<class Iterator>
    bool operator()(Iterator lhs_begin, Iterator lhs_end, const BinaryData& rhs)
    {
        return std::lexicographical_compare(std::execution::par_unseq, lhs_begin, lhs_end, rhs.begin(), rhs.end());
    }

    template<class Iterator>
    bool operator()(const BinaryData& rhs, Iterator lhs_begin, Iterator lhs_end)
    {
        return std::lexicographical_compare(std::execution::par_unseq, rhs.begin(), rhs.end(), lhs_begin, lhs_end);
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
        assert(lhs.size() == rhs.size());
        return std::equal(std::execution::par_unseq, lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<class Iterator>
    bool operator()(Iterator lhs_begin, Iterator lhs_end, const BinaryData& rhs)
    {
        checkIteratorTag<Iterator>();
        size_t dist1 = std::distance(lhs_begin, lhs_end);
        size_t dist2 = rhs.size();
        if( dist1 != dist2 ) return false;
        assert(dist1 == dist2);
        return std::equal(std::execution::par_unseq, lhs_begin, lhs_end, rhs.begin(), rhs.end());
    }
} equal;

// TODO: count 1
struct Subtraction
{
    BinaryData operator()(const BinaryData& minuend, const BinaryData& subtrahend)
    {
        assert(isLoan == false);
        auto [result, it] = proccess(subtrahend.crbegin(), subtrahend.crend(), minuend.crbegin());
        addRest(result, it, minuend.crend());
        return result;
    }

private:
    bool isLoan = false;

    template<class Iterator>
    void addRest(BinaryData& number, Iterator startIt, Iterator endIt)
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
            }
        }

        for(;startIt != endIt; ++startIt)
            number.push_front(*startIt);

        assert(startIt == endIt);
        removeInsignificantBits(number);
        isLoan = false;
    }

    template<class Iterator>
    std::pair<BinaryData, Iterator> proccess(Iterator subtrahendStart, Iterator subtrahendEnd, Iterator minuendIt)
    {
        BinaryData result;
        for(; subtrahendStart != subtrahendEnd; ++subtrahendStart, ++minuendIt)
        {
            if(*minuendIt > *subtrahendStart) // 1 0
            {
                if(isLoan)
                {
                    isLoan = false;
                    result.push_front(0);
                }
                else result.push_front(1);
            }
            else if(*minuendIt < *subtrahendStart) // 0 1
            {
                if(!isLoan)
                {
                    result.push_front(1);
                    isLoan = true;
                }
                else result.push_front(0);
            }
            else // 1 1 or 0 0
            {
                if(isLoan) result.push_front(1);
                else result.push_front(0);
            }
        }

        return {result, minuendIt};
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
    BinaryData operator()(const BinaryData& lhs, const BinaryData& rhs)
    {
        assert(isTransfer == false);
        if(lhs.size() < rhs.size())
        {
            auto [result, it] = proccess(lhs.crbegin(), lhs.crend(), rhs.crbegin());
            addRest(result, it, rhs.crend());
            return result;
        } else if (lhs.size() > rhs.size())
        {
            auto [result, it] = proccess(rhs.crbegin(), rhs.crend(), lhs.crbegin());
            addRest(result, it, lhs.crend());
            return result;
        }
        else
        {
            auto [result, it] = proccess(lhs.crbegin(), lhs.crend(), rhs.crbegin());
            addRest(result, it, rhs.crend());
            return result;
        }
    }

private:
    bool isTransfer = false;

    // TODO: count 1
    template<class Iterator>
    void addRest(BinaryData& number, Iterator startIt, Iterator endIt)
    {
        if(isTransfer)
        {
            for(; startIt != endIt; ++startIt)
            {
                if(*startIt == 0)
                {
                    number.push_front(1);
                    isTransfer = false;
                    ++startIt;
                    break;
                }

                number.push_front(0);
            }
        }

        for(;startIt != endIt; ++startIt)
            number.push_front(*startIt);

        if(isTransfer)
        {
            number.push_front(1);
            isTransfer = false;
        }
    }

    // TODO: count 1
    template<class Iterator>
    std::pair<BinaryData, Iterator> proccess(Iterator lhsIt, Iterator endIt, Iterator rhsIt)
    {
        BinaryData result;
        for(; lhsIt != endIt; ++rhsIt, ++lhsIt)
        {
            if(*lhsIt == 1 && *rhsIt == 1)
            {
                if(isTransfer)
                    result.push_front(1);
                else
                {
                    isTransfer = true;
                    result.push_front(0);
                }
            } else if(*lhsIt == 0 && *rhsIt == 0)
            {
                isTransfer ? result.push_front(1) : result.push_front(0);
                isTransfer = false;
            } else if(*lhsIt != *rhsIt)
                isTransfer ? result.push_front(0) : result.push_front(1);
        }

        return {result, rhsIt};
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
     BinaryData operator()(const BinaryData& lhs, const BinaryData& rhs)
     {
         BinaryData result;

         return result;
     }

} modulo;

struct Multiplication
{
    BinaryData operator()(const BinaryData& lhs, const BinaryData& rhs)
    {
        return proccess(lhs.crbegin(), lhs.crend(), rhs);
    }

private:
    size_t offset = 0;

    void handleNextZero(BinaryData& result, BinaryData& tmp, const BinaryData& other)
    {
        if(offset > 1)
        {
             result = addition(result, tmp);
            if(offset > 1) result = subtraction(result, other);
            offset = 0;
        }
    }

    template<class Iterator>
    BinaryData proccess(Iterator startIt, Iterator endIt, const BinaryData& other)
    {
        BinaryData result;
        BinaryData tmp;
        std::copy(std::execution::par_unseq, startIt, endIt, std::back_inserter(tmp));
        for(; startIt != endIt; ++startIt)
        {
            if(*startIt == 0) leftShift(tmp, 0);
            if(*startIt == 1)
            {
                ++offset;
                if( *std::next(startIt, 1) == 0 || std::next(startIt, 1) == endIt )
                    handleNextZero(result, tmp, other);

                leftShift(tmp, 0);
            }
        }

        return result;
    }

} multiplication;
