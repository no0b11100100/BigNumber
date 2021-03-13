#include <iostream>
#include <chrono>
#include <functional>
#include "BigInt.hpp"

using namespace std;

namespace Timer
{

using ns = std::chrono::nanoseconds;
using mcs = std::chrono::microseconds;
using ms = std::chrono::milliseconds;
using s = std::chrono::seconds;

template <class TimeMeasurement>
class Timer
{
public:

    template<class Callable, class ...Args>
    Timer(Callable&& callable, Args&& ... args)
    {
        auto startTime = std::chrono::system_clock::now();
        std::invoke(callable, std::forward<Args>(args)...);
        auto endTime = std::chrono::system_clock::now();
        m_time = std::chrono::duration_cast<TimeMeasurement>(endTime - startTime).count();
    }

    size_t Time() const { return m_time; }

private:
    size_t m_time;
};

} // Timer

int main()
{
    BigInt::BigInt a(10);
//    BigInt::BigInt b(std::string("10"));

    for(auto v : a.Number()) cout << v << endl;
//    for(auto v : b.Number()) cout << v << endl;

    return 0;
}
