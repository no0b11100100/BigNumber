#include <iostream>
#include <chrono>
#include <functional>
#include "BigInt.hpp"
#include <bitset>

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
        std::invoke(std::forward<Callable>(callable), std::forward<Args>(args)...);
        auto endTime = std::chrono::system_clock::now();
        m_time = std::chrono::duration_cast<TimeMeasurement>(endTime - startTime).count();
    }

    void Time() const { std::cout << m_time << std::endl; }

private:
    size_t m_time;
};

template <class TimeMeasurement>
class ScopedTimer
{
    std::chrono::time_point<std::chrono::system_clock> startTime;
public:

    template<class Callable, class ...Args>
    ScopedTimer(Callable&& callable, Args&& ... args)
    {
        startTime = std::chrono::system_clock::now();
        std::invoke(std::forward<Callable>(callable), std::forward<Args>(args)...);
    }

    ~ScopedTimer()
    {
        std::cout << std::chrono::duration_cast<TimeMeasurement>(std::chrono::system_clock::now() - startTime).count() << std::endl;
    }
};

} // Timer

int main()
{
    return 0;
}
