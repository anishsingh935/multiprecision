#include <chrono>
#include <limits>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace boost_gsoc2020 {

/* Utility class to measure the execution time. */
template <class clock_type>
struct stopwatch
{
 public:
   typedef typename clock_type::duration duration_type;

   stopwatch() : m_start(clock_type::now()) {}

   stopwatch(const stopwatch& other) : m_start(other.m_start) {}

   stopwatch& operator=(const stopwatch& other)
   {
      m_start = other.m_start;
      return *this;
   }

   ~stopwatch() {}

   /* Returns the elapsed time since the timer was last reset. */
   duration_type elapsed() const
   {
      return (clock_type::now() - m_start);
   }

   /* Resets the timer. */
   void reset()
   {
      m_start = clock_type::now();
   }

 private:
   typename clock_type::time_point m_start;
};

using stopwatch_type = stopwatch<std::chrono::high_resolution_clock>;

/* Utility class to compute the mean and variance over independent
   and identically distributed samples. */
template <typename T>
struct Aggregator
{

   Aggregator() : mean(0), variance(0), n(0) {}

   void addMeasurement(T measurement)
   {
      T prev_mean = mean;
      if (n == 0)
      {
         mean = measurement;
      }
      else
      {
         mean += (measurement - mean) / T(n+1);
         variance += (measurement - prev_mean) * (measurement - mean);
      }
      ++n;
   }

   /* Returns the estimated mean. */
   T getMean() const
   {
      return mean;
   }

   /* Returns the estimated variance. */
   T getVariance() const
   {
      return variance / (n - 1);
   }

   T      mean;
   T      variance;
   size_t n;
};

using aggregator_type = Aggregator<long long>;

} // namespace boost_gsoc2020
