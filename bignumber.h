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
        if(number == 0)
        {
            m_number.push_back(0);
            return;
        }

        while(number > 0)
        {
            m_number.push_front(number%10);
            number /= 10;
        }
    }

    void fromString(std::string number)
    {
        validateNumber(number);
        bool isFraction = false;
        for(auto digit : number)
        {
            if(digit == '.') isFraction = true;

            if(!isFraction) m_number.push_front(digit-'0');
            else m_fraction.push_front(digit-'0');
        }
    }

    void fromDouble(double number)
    {
        fromString(std::to_string(number));
    }

    void fromVector(const std::vector<int>& number)
    {
        validateNumber(number);
        for(auto& digit : number)
        {
            m_number.push_front(digit);
        }
    }

    void fromList(const std::list<int>& number)
    {
        validateNumber(number);
        for(auto& digit : number)
        {
            m_number.push_front(digit);
        }
    }

    void fromDeque(const std::deque<int>& number)
    {
        validateNumber(number);
        for(auto& digit : number)
        {
            m_number.push_front(digit);
        }
    }

    // For container
    template<class T>
    void validateNumber(T number)
    {
        bool first = false;
        for(auto digit : number)
        {
            if(first == false)
            {
                first = true;
                if(digit > 9 || digit < -9) throw std::runtime_error("Invalid number");
            }
            if(digit > 9 || digit < 0) throw std::runtime_error("Invalid number");
        }
    }

    void validateNumber(std::string number)
    {
        char fisrtDigit = number[0];

        if(fisrtDigit < '0' || fisrtDigit > '9' || fisrtDigit != '-') throw std::runtime_error("Invalid number");
        if(fisrtDigit == '-' && number[1] == '.') throw std::runtime_error("Invalid number");

        for(std::size_t i = 1, dotCounter = 0; i < number.length(); ++i)
        {
            if(number[i] < '0' || number[i] > '9' || number[i] != '.') throw std::runtime_error("Invalid number");
            if(number[i] == '.')
            {
                ++dotCounter;
                if(dotCounter > 1) throw std::runtime_error("Invalid number");
            }
        }
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

    //TODO: remake to fit my needs
    int mod(std::string num, int a)
    {
        // Initialize result
        int res = 0;

        // One by one process all digits of 'num'
        for (std::size_t i = 0; i < num.length(); i++)
             res = (res*10 + (int)num[i] - '0') %a;

        return res;
    }

    //TODO: remake to fit my needs
    std::string longDivision(std::string number, int divisor)
    {
        std::string ans;
        std::size_t idx = 0;
        int temp = number[idx] - '0';
        while (temp < divisor)
            temp = temp * 10 + (number[++idx] - '0');

        // Repeatedly divide divisor with temp. After
        // every division, update temp to include one
        // more digit.
        while (number.size() > idx) {
            // Store result in answer i.e. temp / divisor
            ans += (temp / divisor) + '0';

            // Take next digit of number
            temp = (temp % divisor) * 10 + number[++idx] - '0';
        }

        // If divisor is greater than number
        if (ans.length() == 0)
            return "0";

        // else return ans
        return ans;
    }

    BigNumber& operator-(BigNumber);
    BigNumber& operator*(BigNumber);
    BigNumber operator/(BigNumber);
    BigNumber& operator%(BigNumber);

//    template<typename T>
//    BigNumber& pow(BigNumber, T degree);

//    template<typename T, typename Mod>
//    BigNumber& pow_mod(BigNumber, T degree, Mod modulo);

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
