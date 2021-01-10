#include "bignumber.h"
#include <iostream>
#include <algorithm>

BigNumber::BigNumber::BigNumber(int number)
{
    fromInt(number);
}

BigNumber::BigNumber::BigNumber(std::size_t number)
{
    fromInt(number);
}

BigNumber::BigNumber::BigNumber(float number)
{
    fromDouble(number);
}

BigNumber::BigNumber::BigNumber(std::string number)
{
    fromString(number);
}

BigNumber::BigNumber::BigNumber(double number)
{
    fromDouble(number);
}

BigNumber::BigNumber &BigNumber::BigNumber::operator-(BigNumber number)
{
    auto num = number.toList();

    auto main_start = rbegin(num);
    auto main_end = rend(num);
    auto second_start = rbegin(m_number);
    auto second_end = rend(m_number);

    bool isBigger = false;
    if (*this < number)
    {
        int diff = num.size() - m_number.size();
        for(int i = 0; i < diff; ++i)
        {
            m_number.push_front(0);
        }
        isBigger = true;
        main_start = rbegin(m_number); // less
        main_end = rend(m_number); // less
        second_start = rbegin(num);
        second_end = rend(num);
        std::cout << "handle\n";
    }

    for(; main_start != main_end; ++second_start, ++main_start)
    {
        if (*second_start < *main_start)
        {
            *second_start += (10 - *main_start);
            if(isBigger) *main_start = *second_start;

            for(auto i = std::next(second_start, 1); i != second_end; ++i)
            {
                if(*i == 0) *i = 9;
                else
                {
                    *i -= 1;
                    break;
                }
            }

            std::prev(second_start, 1);
        }
        else
        {
            *second_start -= *main_start;
            if(isBigger) *main_start = *second_start;
        }
    }

    for(auto it = begin(m_number); it != end(m_number);)
    {
        if(*it == 0)
            it = m_number.erase(it);
        else
        {
            ++it;
            break;
        }
    }
    return *this;

}

bool BigNumber::BigNumber::operator<(BigNumber other)
{
    if (m_number.size() < other.toList().size()) return true;
    else if (m_number.size() > other.toList().size()) return false;

    auto it = this->toList();
    auto it_other = other.toList();

    for(std::size_t i = 0; i < it.size(); ++i)
    {
        auto first_digit = std::next(it.begin(), i);
        auto second_digit = std::next(it_other.begin(), i);

        if(*first_digit < *second_digit) return true;
        if(*first_digit > *second_digit) return false;
    }

    return false;

}


BigNumber::BigNumber& BigNumber::BigNumber::operator+(int number)
{
    BigNumber a(number);
    return *this + a;
}

BigNumber::BigNumber& BigNumber::BigNumber::operator+(BigNumber number)
{
    auto num = number.toList();

    if(m_number.size() < num.size())
    {
        int diff = num.size() - m_number.size();
        for(int i = 0; i < diff; ++i)
        {
            m_number.push_front(0);
        }
    }

    auto it = rbegin(m_number);
    auto it_num = rbegin(num);

    for(; it_num != rend(num); ++it, ++it_num)
    {
        int digit = *it + *it_num;
        if(digit > 9)
        {
            *it = digit - 10;
            *(std::next(it, 1)) += 1;
            int count = 0;
            for(auto i = it; i != rend(m_number); ++i)
            {
                if(*i > 9)
                {
                    ++count;
                    *i = digit - 10;
                    *(std::next(i, 1)) += 1;
                }
                else { break; }
            }
            std::prev(it, count+1);
        }
        else
        {
            *it = digit;
        }
    }

    return *this;
}

std::string BigNumber::BigNumber::toString() const
{
    std::string number(m_number.size(), '0');

    int i = 0;
    for(const auto& digit : m_number)
    {
        number[i] = digit + '0';
        ++i;
    }

    return number;
}

std::list<int> BigNumber::BigNumber::toList() const
{
    return m_number;
}

std::vector<int> BigNumber::BigNumber::toVector() const
{
    std::vector<int> result;
    result.reserve(m_number.size());
    for(const auto& digit : m_number)
    {
        result.push_back(digit);
    }

    return result;
}

std::deque<int> BigNumber::BigNumber::toDeque() const
{
    std::deque<int> result;
    for(const auto& digit : m_number)
    {
        result.push_back(digit);
    }

    return result;
}
