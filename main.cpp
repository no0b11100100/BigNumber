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
        m_time = std::chrono::duration_cast<TimeMeasurement>(std::chrono::system_clock::now() - startTime).count();
    }

    void Time() const { std::cout << "time is " << m_time << std::endl; }

private:
    size_t m_time;
};

template <class TimeMeasurement>
class ScopedTimer
{
    std::chrono::time_point<std::chrono::system_clock> startTime;

    std::string TimeMeasurementToString()
    {
        return std::is_same_v<TimeMeasurement, ns> ? " ns" :
               std::is_same_v<TimeMeasurement, mcs> ? " mcs" :
               std::is_same_v<TimeMeasurement, ms> ? " ms" : " s";
    }

public:

    ScopedTimer()
    {
        startTime = std::chrono::system_clock::now();
    }

    ~ScopedTimer()
    {
        std::cout << std::chrono::duration_cast<TimeMeasurement>(std::chrono::system_clock::now() - startTime).count()
                  << TimeMeasurementToString() << std::endl;
    }
};

} // Timer

int main()
{
    return 0;
}
