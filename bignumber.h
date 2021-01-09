#pragma once
#include <list>
#include <vector>
#include <string>
#include <iostream>

namespace BigNumber
{
class BigNumber
{
    std::list<int> m_number;
public:
    BigNumber(int number);

    BigNumber& operator+(BigNumber);
    BigNumber& operator-(BigNumber);
    BigNumber& operator*(BigNumber);
    BigNumber& operator/(BigNumber);
    BigNumber& operator%(BigNumber);
    BigNumber& operator<(BigNumber);
    BigNumber& operator<=(BigNumber);
    BigNumber& operator>(BigNumber);
    BigNumber& operator>=(BigNumber);
    BigNumber& operator==(BigNumber);
    BigNumber& operator!=(BigNumber);
    std::ostream& operator<<(BigNumber);
    std::istream& operator>>(int);

    std::string toString() const;
    std::list<int> toList() const;
    std::vector<int> toVector() const;
};

} // BigNumber
