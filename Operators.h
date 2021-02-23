#pragma once
#include <deque>
#include <execution>
#include <algorithm>

using BinaryData = std::deque<bool>;

struct Less
{
    template<class BeginIterator, class EndIterator>
    bool operator()(BeginIterator lhs_begin, BeginIterator lhs_end, EndIterator rhs_begin, EndIterator rhs_end)
    {
        auto [lhs_it, rhs_it] = std::mismatch(std::execution::par_unseq, lhs_begin, lhs_end, rhs_begin, rhs_end);
        return *lhs_it < *rhs_it;
    }
} static less;

struct Greater
{
    template<class BeginIterator, class EndIterator>
    bool operator()(BeginIterator lhs_begin, BeginIterator lhs_end, EndIterator rhs_begin, EndIterator rhs_end)
    {
        auto [lhs_it, rhs_it] = std::mismatch(std::execution::par_unseq, lhs_begin, lhs_end, rhs_begin, rhs_end);
        return *lhs_it > *rhs_it;
    }
} static greater;

struct Equal
{
    template<class BeginIterator, class EndIterator>
    bool operator()(BeginIterator lhs_begin, BeginIterator lhs_end, EndIterator rhs_begin, EndIterator rhs_end)
    {
        return std::equal(std::execution::par_unseq, lhs_begin, lhs_end, rhs_begin, rhs_end);
    }
} static equal;

struct Subtraction
{
    template<class BeginIterator, class EndIterator>
    BinaryData operator()(BeginIterator lhs_begin, BeginIterator lhs_end, EndIterator rhs_begin, EndIterator rhs_end)
    {
        BinaryData result;
        size_t bits = 0;
        bool isLoan = false;
        std::size_t size = std::max(
                    std::distance(lhs_begin, lhs_end),
                    std::distance(rhs_begin, rhs_end) );

        return result;
    }
} static subtraction;

struct Addition
{
    template<class BeginIterator, class EndIterator>
    BinaryData operator()(BeginIterator lhs_begin, BeginIterator lhs_end, EndIterator rhs_begin, EndIterator rhs_end)
    {
        BinaryData result;
        size_t bits = 0;
        bool isTransfer = false;
        std::size_t size = std::max(
                    std::distance(lsh_begin, lhs_end),
                    std::distance(rhs_begin, rhs_end) );

        return result;
    }
} static addition;

struct Division
{
    template<class BeginIterator, class EndIterator>
    BinaryData operator()(BeginIterator lhs_begin, BeginIterator lhs_end, EndIterator rhs_begin, EndIterator rhs_end)
    {
        BinaryData result;
        return result;
    }

} static division;

struct Multiplication
{
    template<class BeginIterator, class EndIterator>
    BinaryData operator()(BeginIterator lhs_begin, BeginIterator lhs_end, EndIterator rhs_begin, EndIterator rhs_end)
    {
        BinaryData result;

        return result;
    }

} static multiplication;
