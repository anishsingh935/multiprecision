/*
Contains the performance tests for the logarithm implementation.
 */
#include <chrono>
#include <limits>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <boost/math/constants/constants.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <random>

#include "boost_gsoc_benchmark_utils.h"

template <typename Generator>
std::string generateRandomNumber(size_t digits, Generator& gen)
{
   std::uniform_int_distribution<int> uniform_digit(0, 10);
   std::uniform_int_distribution<size_t> uniform_point(1, digits);
   std::string                        str;
   str += std::to_string(uniform_digit(gen) % 9 + 1);
   size_t point = uniform_point(gen);
   for (size_t i = 1; i < digits; ++i)
   {
      str += std::to_string(uniform_digit(gen));
      if (i == point)
         str += '.';
   }
   return str;
}

template <typename T>
void run_time_experiments(size_t cur_digits)
{
    size_t                     reps = 20;
    std::default_random_engine generator(42);
    int                        total = 0;
    boost_gsoc2020::aggregator_type aggregator;
    for (size_t rep = 0; rep < reps; ++rep)
    {
        T                              val(generateRandomNumber(cur_digits, generator));
        boost_gsoc2020::stopwatch_type stopwatch;
        stopwatch.reset();
        T ans = log(val);
        total += !ans.is_zero();
        auto time = stopwatch.elapsed().count();
        aggregator.addMeasurement(time);
    }
    double mean         = aggregator.getMean() / 1000000000.0;
    double st_deviation = sqrt(aggregator.getVariance()) / 1000000000.0;
    std::cout << "(" << cur_digits << ", " << mean << ") +- ("
              << st_deviation << ", " << 1.96 * st_deviation << ")" << std::endl;
}

template<size_t Digits>
void run_time_experiment_for_cpp_bin_float() {
   run_time_experiments<boost::multiprecision::number<boost::multiprecision::backends::cpp_bin_float<Digits> > >(Digits);
}

template <size_t Digits>
void run_time_experiment_for_cpp_dec_float()
{
   run_time_experiments<boost::multiprecision::number<boost::multiprecision::backends::cpp_dec_float<Digits> > >(Digits);
}

int main()
{
   
   std::cout << "==== Running tests for cpp_bin_float: ====" << std::endl;
   run_time_experiment_for_cpp_bin_float<1000>();
   run_time_experiment_for_cpp_bin_float<1500>();
   run_time_experiment_for_cpp_bin_float<2000>();
   run_time_experiment_for_cpp_bin_float<2500>();
   run_time_experiment_for_cpp_bin_float<3000>();
   run_time_experiment_for_cpp_bin_float<3500>();
   run_time_experiment_for_cpp_bin_float<4000>();
   run_time_experiment_for_cpp_bin_float<4500>();
   run_time_experiment_for_cpp_bin_float<5000>();
   run_time_experiment_for_cpp_bin_float<5500>();
   run_time_experiment_for_cpp_bin_float<6000>();
   run_time_experiment_for_cpp_bin_float<6500>();
   run_time_experiment_for_cpp_bin_float<7000>();
   run_time_experiment_for_cpp_bin_float<7500>();
   run_time_experiment_for_cpp_bin_float<8000>();
   run_time_experiment_for_cpp_bin_float<8500>();
   run_time_experiment_for_cpp_bin_float<9000>();
   run_time_experiment_for_cpp_bin_float<9500>();
   run_time_experiment_for_cpp_bin_float<10000>();

   /* std::cout << "==== Running tests for cpp_dec_float: ====" << std::endl;
   run_time_experiment_for_cpp_dec_float<1000>();
   run_time_experiment_for_cpp_dec_float<1500>();
   run_time_experiment_for_cpp_dec_float<2000>();
   run_time_experiment_for_cpp_dec_float<2500>();
   run_time_experiment_for_cpp_dec_float<3000>();
   run_time_experiment_for_cpp_dec_float<3500>();
   run_time_experiment_for_cpp_dec_float<4000>();
   run_time_experiment_for_cpp_dec_float<4500>();
   run_time_experiment_for_cpp_dec_float<5000>();
   run_time_experiment_for_cpp_dec_float<5500>();
   run_time_experiment_for_cpp_dec_float<6000>();
   run_time_experiment_for_cpp_dec_float<6500>();
   run_time_experiment_for_cpp_dec_float<7000>();
   run_time_experiment_for_cpp_dec_float<7500>();
   run_time_experiment_for_cpp_dec_float<8000>();
   run_time_experiment_for_cpp_dec_float<8500>();
   run_time_experiment_for_cpp_dec_float<9000>();
   run_time_experiment_for_cpp_dec_float<9500>();
   run_time_experiment_for_cpp_dec_float<10000>();
   */
   return 0;
}
