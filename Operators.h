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

struct Less
{
    bool operator()(BinaryData lhs, BinaryData rhs)
    {
        if( lhs.size() > rhs.size() ) return false;
        if( lhs.size() < rhs.size() ) return true;
        assert(lhs.size() == rhs.size());
        auto [lhs_it, rhs_it] = std::mismatch(std::execution::par_unseq, lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
        return rhs_it == rhs.end();
    }

    template<class Iterator>
    bool operator()(Iterator lhs_begin, Iterator lhs_end, BinaryData rhs)
    {
        size_t dist1 = std::distance(lhs_begin, lhs_end);
        size_t dist2 = rhs.size();
        if( dist1 > dist2 ) return false;
        if( dist1 < dist2 ) return true;
        assert(dist1 == dist2);
        auto [lhs_it, rhs_it] = std::mismatch(std::execution::par_unseq, lhs_begin, lhs_end, rhs.begin(), rhs.end());
        return rhs_it == rhs.end();
    }
} less;

struct Greater
{
    bool operator()(BinaryData lhs, BinaryData rhs)
    {
        if( rhs.size() > lhs.size() ) return false;
        if( rhs.size() < lhs.size() ) return true;
        assert(lhs.size() == rhs.size());
        auto [lhs_it, rhs_it] = std::mismatch(std::execution::par_unseq, lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
        return rhs_it == rhs.end();
    }

    template<class Iterator>
    bool operator()(Iterator lhs_begin, Iterator lhs_end, BinaryData rhs)
    {
        size_t dist1 = std::distance(lhs_begin, lhs_end);
        size_t dist2 = rhs.size();
        if( dist2 > dist1 ) return false;
        if( dist2 < dist1 ) return true;
        assert(dist1 == dist2);
        auto [lhs_it, rhs_it] = std::mismatch(std::execution::par_unseq, lhs_begin, lhs_end, rhs.begin(), rhs.end());
        return rhs_it == rhs.end();
    }
} greater;

struct Equal
{
    bool operator()(BinaryData lhs, BinaryData rhs)
    {
        if( lhs.size() != rhs.size() ) return false;
        assert(lhs.size() == rhs.size());
        return std::equal(std::execution::par_unseq, lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<class Iterator>
    bool operator()(Iterator lhs_begin, Iterator lhs_end, BinaryData rhs)
    {
        size_t dist1 = std::distance(lhs_begin, lhs_end);
        size_t dist2 = rhs.size();
        if( dist1 != dist2 ) return false;
        assert(dist1 == dist2);
        return std::equal(std::execution::par_unseq, lhs_begin, lhs_end, rhs.begin(), rhs.end());
    }
} equal;

struct Subtraction
{
    BinaryData operator()(BinaryData lhs, BinaryData rhs)
    {
        BinaryData result;

        return result;
    }
} subtraction;

struct Addition
{
    BinaryData operator()(BinaryData lhs, BinaryData rhs)
    {
        BinaryData result;

        return result;
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

struct Multiplication
{
    BinaryData operator()(BinaryData lhs, BinaryData rhs)
    {
        BinaryData result;

        return result;
    }

} multiplication;
