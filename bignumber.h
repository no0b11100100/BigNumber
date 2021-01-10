#pragma once
#include <list>
#include <vector>
#include <deque>
#include <string>
#include <iostream>
#include <exception>

namespace BigNumber
{
class BigNumber
{
    std::list<int> m_number;
    std::list<int> m_fraction;

    template<typename T, class = typename std::enable_if_t<std::is_same_v<T, int> || std::is_same_v<T, std::size_t> > >
    void fromInt(T number)
    {
        while(number > 0)
        {
            m_number.push_front(number%10);
            number /= 10;
        }
    }

    void fromString(std::string number)
    {
        int dotCount = 0;
        bool isFraction = false;
        for(auto digit : number)
        {
            if(digit == '.')
            {
                ++dotCount;
                if (dotCount > 1) throw std::runtime_error("Invalid number. Number is not int or double. Got " + number);
                isFraction = true;
            }

            if(!isFraction) m_number.push_front(digit-'0');
            else m_fraction.push_front(digit-'0');
        }
    }

    void fromDouble(double number)
    {
        fromString(std::to_string(number));
    }

public:
    BigNumber(int number);
    BigNumber(std::size_t number);
    BigNumber(double number);
    BigNumber(float number);
    BigNumber(std::string number);

    BigNumber& operator+(BigNumber);
    BigNumber& operator+(int);
    BigNumber& operator+(std::size_t);
    BigNumber& operator+(double);
    BigNumber& operator+(float);
    BigNumber& operator+(std::string);

    BigNumber& operator-(BigNumber);
    BigNumber& operator*(BigNumber);
    BigNumber& operator/(BigNumber);
    BigNumber& operator%(BigNumber);
    bool operator<(BigNumber);
    bool operator<=(BigNumber);
    bool operator>(BigNumber);
    bool operator>=(BigNumber);
    bool operator==(BigNumber);
    bool operator!=(BigNumber);
    std::ostream& operator<<(BigNumber);
    std::istream& operator>>(int);

    std::string toString() const;
    std::list<int> toList() const;
    std::vector<int> toVector() const;
    std::deque<int> toDeque() const;
};

} // BigNumber
