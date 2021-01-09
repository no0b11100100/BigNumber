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

BigNumber::BigNumber::BigNumber(double number)
{
    fromDouble(number);
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
