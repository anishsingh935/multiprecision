///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 1999-2020.
//  Distributed under the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// This program contains implementations of the following
// algorithms for computing pi:
//  * Gauss-Legendre (implementation based on Pi Unleashed)
//  * Cubic-order Borwein (implementation based on Pi Unleashed)
//  * Gauss-Legendre (basic implementation)
//  * Quadratic-order Borwein
//  * Cubic-order Borwein
//  * Quartic-order Borwein
//  * Quintic-order Borwein
//  * Nonic-order Borwein
// This includes correctness and performance tests.

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <string>

#include "boost_gsoc_benchmark_utils.h"
#include "pi_digits.h"

#include <boost/lexical_cast.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
// #include <boost/multiprecision/gmp.hpp>

// 0: cpp_bin
// 1: cpp_dec
// 2: gmp
#define FLOAT_TYPE 0

#if FLOAT_TYPE == 0
template<unsigned int Digits>
using float_type =
    boost::multiprecision::number<boost::multiprecision::cpp_bin_float<Digits>,
                                  boost::multiprecision::et_off>;
#elif FLOAT_TYPE == 1
template <unsigned int Digits>
using float_type =
    boost::multiprecision::number<boost::multiprecision::cpp_dec_float<Digits>,
                                  boost::multiprecision::et_off>;
#elif FLOAT_TYPE == 2
template <unsigned int Digits>
using float_type =
    boost::multiprecision::number<boost::multiprecision::gmp_float<Digits>,
                                  boost::multiprecision::et_off>;
#endif

namespace pi { namespace millions { namespace detail {

template <typename T>
inline T sqr(const T& a)
{
   return a * a;
}

template <typename T>
inline T cube(const T& a)
{
   return a * a * a;
}

/**
// \func template<typename float_type> float_type gauss_legendre_pi_unleashed()
//
// \desc Compute pi using a quadratically convergent Gauss AGM,
//       in the Schoenhage variant. This is a well-known algorithm.
//       For a particularly clear description of this algorithm,
//       see "Algorithm 16.148" in Chapter 16 on page 236 from
//       the book "Pi Unleashed" by J. Arndt and C. Haenel
//       (Springer Verlag, Heidelberg, 2001).
//
*/
template <typename T>
T gauss_legendre_pi_unleashed()
{
   // Start with (2 * x^2) = digits10 and solve for x.
   static const float f_iter = std::log(0.5F * float(std::numeric_limits<T>::digits10)) / std::log(2.0F);

   static const std::size_t max_iter = 1U + static_cast<std::size_t>(f_iter);

   static bool is_init;

   static T val_pi;

   if (!is_init)
   {
      is_init = true;

      T a(1.0F);
      T bB(0.5F);
      T s(0.5F);
      T t(0.375F);

      for (std::size_t i = 0; i < max_iter; ++i)
      {
         using std::sqrt;

         a += sqrt(bB);
         a /= 2U;
         val_pi = a;
         val_pi *= val_pi;
         bB = (val_pi - t);
         bB *= 2U;

         const T iterate_term((bB - val_pi) * (1ULL << (i + 1U)));

         s += iterate_term;

         t = val_pi;
         t += bB;
         t /= 4U;
      }

      val_pi += bB;
      val_pi /= s;
   }

   return val_pi;
}

/**
// \func template<typename float_type> float_type cubic_borwein_pi_unleashed()
//
// \desc Compute pi using a cubically convergent iteration scheme.
//       See the book "Pi Unleashed", algorithm 16.151, page 237.
//
*/
template <typename T>
T cubic_borwein_pi_unleashed()
{
   // Start with (2 * x^3) = digits10 and solve for x.
   static const float f_iter = std::log(0.5F * float(std::numeric_limits<T>::digits10)) / std::log(3.0F);

   static const std::size_t max_iter = 1U + static_cast<std::size_t>(f_iter);

   static bool is_init = false;

   static T val_pi;

   if (!is_init)
   {
      is_init = true;

      val_pi = 1;

      T three(3);
      T a_n(T(1) / 3);
      T s_n((sqrt(three) - 1) / 2);
      T pow_3(1);

      for (std::size_t i = 0; i < max_iter; ++i)
      {
         T s_n_cubed = (s_n * s_n) * s_n;

         T r_n = 3 / (1 + (2 * cbrt(1 - s_n_cubed)));

         const T r_n_squared = r_n * r_n;

         s_n = (r_n - 1) / 2;

         a_n = (r_n_squared * a_n) - (pow_3 * (r_n_squared - 1));

         pow_3 *= 3;
      }

      val_pi = 1 / a_n;
   }

   return val_pi;
}

/**
// \func template<typename float_type> float_type& quadratic_borwein_for_pi()
//
// \desc Compute pi using a quadratically convergent iteration scheme.
//       See Algorithm 2.1 on p.46 of
//          Borwein, Jonathan M., and Peter B. Borwein. "Pi and the AGM: a study in 
//              the analytic number theory and computational complexity". 
//              Wiley-Interscience, 1987.
//
*/
template <typename T>
T quadratic_borwein_for_pi()
{
   static const float f_iter = std::log2(0.5F * float(std::numeric_limits<T>::digits10));

   static const std::size_t max_iter = 1U + static_cast<std::size_t>(f_iter);
   T                        two(2);
   T                        sqrt_2(sqrt(two));
   T                        a_n = sqrt_2, b_n(0), p_n(2 + sqrt_2);
   T                        sqrt_a_n, new_a_n, new_b_n;
   for (size_t i = 0; i < max_iter; ++i)
   {
      sqrt_a_n = sqrt(a_n);
      new_a_n  = (sqrt_a_n + 1 / sqrt_a_n) / 2;
      new_b_n  = (1 + b_n) * sqrt_a_n / (a_n + b_n);
      a_n      = new_a_n;
      b_n      = new_b_n;
      p_n      = (1 + a_n) * p_n * b_n / (1 + b_n);
   }
   return p_n;
}

/**
// \func template<typename float_type> float_type& cubic_borwein_for_pi()
//
// \desc Compute pi using a cubically convergent iteration scheme.
//       See p.47 of
//          Borwein, Jonathan M., Peter B. Borwein, and Frank G. Garvan. 
//             "Some cubic modular identities of Ramanujan." Transactions 
//             of the American Mathematical Society (1994): 35-47.
//
*/
template <typename T>
T cubic_borwein_for_pi()
{
   const float f_iter = std::log(0.5F * float(std::numeric_limits<T>::digits10)) / std::log(3.0F);

   const std::size_t max_iter = 1U + static_cast<std::size_t>(f_iter);

   T three(3);
   T a_n(1 / three), s_n((sqrt(three) - 1) / 2), pow_3(1), r_n;
   for (size_t i = 0; i < max_iter; ++i)
   {
      r_n = 3 / (1 + 2 * cbrt(1 - cube(s_n)));
      s_n = (r_n - 1) / 2;
      r_n *= r_n;
      a_n = r_n * (a_n - pow_3) + pow_3;
      pow_3 *= 3;
   }
   return 1 / a_n;
}

/**
// \func template<typename float_type> float_type& quartic_borwein_for_pi()
//
// \desc Compute pi using a quartically convergent iteration scheme.
//       See Algorithm 5.3 on p.170 of
//          Borwein, Jonathan M., and Peter B. Borwein. "Pi and the AGM: a study in 
//              the analytic number theory and computational complexity". 
//              Wiley-Interscience, 1987.
//
*/
template <typename T>
T quartic_borwein_for_pi()
{
   const float f_iter = std::log2(0.5F * float(std::numeric_limits<T>::digits10)) / 2.0F;

   const std::size_t max_iter = 1U + static_cast<std::size_t>(f_iter);
   T                 two(2);
   T                 sqrt_2(sqrt(two));
   T                 y_n(sqrt_2 - 1);
   T                 a_n(2 * sqr(y_n)), p_n(8);
   for (size_t i = 0; i < max_iter; ++i)
   {
      T y_n_4       = sqr(sqr(y_n));
      T fourth_root = sqrt(sqrt(1 - y_n_4));
      y_n           = (1 - fourth_root) / (1 + fourth_root);
      a_n           = a_n * sqr(sqr(1 + y_n)) - p_n * y_n * (1 + y_n + sqr(y_n));
      p_n *= 4;
   }
   return 1 / a_n;
}

/**
// \func template<typename float_type> float_type& quintic_borwein_for_pi()
//
// \desc Compute pi using a quintically convergent iteration scheme.
//       See Algorithm 2 on p.170 of
//          Borwein, J.M.; Borwein, P.B. "Approximating $\pi$ with Ramanujan's
//             solvable modular equations". Rocky Mountain J. Math. 19 (1989), no. 1
//
*/
template <typename T>
T quintic_borwein_for_pi()
{
   const float f_iter = std::log2(0.5F * float(std::numeric_limits<T>::digits10)) / std::log2(5.0F);

   const std::size_t max_iter = 1U + static_cast<std::size_t>(f_iter);
   T                 two(2);
   T                 five(5);
   T                 inverse_five(1 / five);
   T                 sqrt_5(sqrt(five));
   T                 a_n(1 / two), s_n(5 * (sqrt_5 - 2));
   T                 pow_five(1);
   T                 s_n2;
   for (size_t i = 0; i < max_iter; ++i)
   {
      T x_n = 5 / s_n - 1;
      T y_n = sqr(x_n - 1) + 7;
      T z_n = kth_root(x_n / 2 * (y_n + sqrt(sqr(y_n) - 4 * cube(x_n))), 5);
      // T z_n = pow(x_n / 2 * (y_n + sqrt(sqr(y_n) - 4 * cube(x_n))), inverse_five);
      s_n2 = sqr(s_n);
      a_n  = s_n2 * a_n - pow_five * ((s_n2 - 5) / 2 + sqrt(s_n * (s_n2 - 2 * s_n + 5)));
      s_n  = 25 / (sqr(z_n + x_n / z_n + 1) * s_n);
      pow_five *= 5;
   }
   return 1 / a_n;
}

/**
// \func template<typename float_type> float_type& nonic_borwein_for_pi()
//
// \desc Compute pi using a nonically convergent iteration scheme.
//       See 
//          Bailey, David H., et al. "The quest for pi." The Mathematical
//             Intelligencer 19 (1997): 50-56.
//
*/
template <typename T>
T nonic_borwein_for_pi()
{
   const float f_iter = std::log2(0.5F * float(std::numeric_limits<T>::digits10)) / std::log2(9.0F);

   const std::size_t max_iter     = 2U + static_cast<std::size_t>(f_iter);
   T                 three        = 3.0;
   T                 inv_three    = 1.0 / three;
   T                 pow_of_three = inv_three;
   T                 a_n = inv_three, r_n = (sqrt(three) - 1) / 2;
   T                 s_n = cbrt(1 - r_n * r_n * r_n);

   for (size_t i = 0; i < max_iter; ++i)
   {
      T t_n = 1 + 2 * r_n;
      T u_n = cbrt(9 * r_n * (1 + r_n + sqr(r_n)));
      T v_n = sqr(t_n) + t_n * u_n + sqr(u_n);
      T w_n = 27 * (1 + s_n + sqr(s_n)) / v_n;
      a_n   = w_n * a_n + pow_of_three * (1 - w_n);
      s_n   = cube(1 - r_n) / ((t_n + 2 * u_n) * v_n);
      r_n   = cbrt(1 - cube(s_n));
      pow_of_three *= 9.0;
   }

   return 1 / a_n;
}

/**
// \func template<typename float_type> float_type& quintic_borwein_for_pi()
//
// \desc Compute pi using Chudnovsky's algorithm.
//       See
//          Chudnovsky, David; Chudnovsky, Gregory (1988), "Approximation 
//             and complex multiplication according to ramanujan", Ramanujan 
//             revisited: proceedings of the centenary conference
//
*/
template <typename T>
T chudnovsky_for_pi()
{
   const float f_iter = float(std::numeric_limits<T>::digits10) / 14.0F;

   const std::size_t max_iter = 1U + static_cast<std::size_t>(f_iter);
   T k(6), m(1), l(13591409), x(1), s(13591409), k3;
   for (size_t i = 0; i < max_iter; ++i)
   {
      k3 = cube(k);
      m  = (k3 - 16 * k) * m / (cube(i + 1));
      l += 545140134;
      x *= -262537412640768000LL;
      s += m * l / x;
      k += 12;
   }
   T sqrt_10005(sqrt(10005));
   return 426880 * sqrt_10005 / s;
}

template <typename T>
T gauss_legendre_for_pi()
{
   static const float f_iter = std::log2(0.5F * float(std::numeric_limits<T>::digits10));

   static const std::size_t max_iter = 1U + static_cast<std::size_t>(f_iter);
   T                        two(2);
   T                        a_n(1), b_n(1.0 / sqrt(two)), t_n(0.25), p_n(1.0);

   for (size_t i = 0; i < max_iter; ++i)
   {
      T new_a_n = (a_n + b_n) / 2;
      t_n       = t_n - p_n * sqr(new_a_n - a_n);
      b_n       = sqrt(a_n * b_n);
      a_n       = new_a_n;
      p_n *= 2;
   }

   return sqr(a_n + b_n) / (4 * t_n);
}

template <unsigned int Digits, float_type<Digits> fn(void)>
void run_experiment(const std::string& name)
{
   std::cout << " Running : " << name << std::endl;

   boost_gsoc2020::stopwatch_type timer;
   timer.reset();

   const float_type<Digits> my_pi = fn();

   const float elapsed = timer.elapsed().count() * 1e-9;
   std::cout << "    Computation time : " << elapsed << "s" << std::endl;
   
   const float_type<Digits> pi_control(test_data_control());

   const float_type<Digits> relative = fabs(float_type<Digits>(1U) - (my_pi / pi_control));

   const bool my_pi_is_ok = relative < std::numeric_limits<float_type<Digits>>::epsilon() * 1e10;

   std::cout << std::setprecision(4) << relative << std::endl;

   std::cout << "my_pi_is_ok: " << std::boolalpha << my_pi_is_ok << std::endl;
}

template <typename T, T ComputePi(void)>
void run_time_experiments(size_t digits)
{
   size_t                          reps = 10;
   int                             total = 0;
   boost_gsoc2020::aggregator_type aggregator;
   for (size_t rep = 0; rep < reps; ++rep)
   {
      boost_gsoc2020::stopwatch_type stopwatch;
      stopwatch.reset();
      T ans = ComputePi();
      total += !ans.is_zero();
      auto time = stopwatch.elapsed().count();
      aggregator.addMeasurement(time);
   }
   double mean         = aggregator.getMean() / 1000000000.0;
   double st_deviation = sqrt(aggregator.getVariance()) / 1000000000.0;
   std::cout << "(" << digits << ", " << mean << ") +- ("
             << 1.96 * st_deviation << ", " << 1.96 * st_deviation << ")" << std::endl;
}
   
}}} // namespace pi::millions::detail

#define RUN_ALL_TIMED_EXPERIMENTS(t, method)                      \
   pi::millions::detail::run_time_experiments<t<1000>, method>(1000); \
   pi::millions::detail::run_time_experiments<t<1500>, method>(1500); \
   pi::millions::detail::run_time_experiments<t<2000>, method>(2000); \
   pi::millions::detail::run_time_experiments<t<2500>, method>(2500); \
   pi::millions::detail::run_time_experiments<t<3000>, method>(3000); \
   pi::millions::detail::run_time_experiments<t<3500>, method>(3500); \
   pi::millions::detail::run_time_experiments<t<4000>, method>(4000); \
   pi::millions::detail::run_time_experiments<t<4500>, method>(4500); \
   pi::millions::detail::run_time_experiments<t<5000>, method>(5000); \
   pi::millions::detail::run_time_experiments<t<5500>, method>(5500); \
   pi::millions::detail::run_time_experiments<t<6000>, method>(6000); \
   pi::millions::detail::run_time_experiments<t<6500>, method>(6500); \
   pi::millions::detail::run_time_experiments<t<7000>, method>(7000); \
   pi::millions::detail::run_time_experiments<t<7500>, method>(7500); \
   pi::millions::detail::run_time_experiments<t<8000>, method>(8000); \
   pi::millions::detail::run_time_experiments<t<8500>, method>(8500); \
   pi::millions::detail::run_time_experiments<t<9000>, method>(9000); \
   pi::millions::detail::run_time_experiments<t<9500>, method>(9500); \
   pi::millions::detail::run_time_experiments<t<10000>, method>(10000);

int main()
{
   const unsigned int digits = 10000;
   
   pi::millions::detail::run_experiment<digits, pi::millions::detail::gauss_legendre_pi_unleashed>("Base pi");
   pi::millions::detail::run_experiment<digits, pi::millions::detail::cubic_borwein_pi_unleashed>("Base cubic");

   pi::millions::detail::run_experiment<digits, pi::millions::detail::gauss_legendre_for_pi>("Guass-Legendre algorithm");
   pi::millions::detail::run_experiment<digits, pi::millions::detail::quadratic_borwein_for_pi>("Quadratic Borwein");
   pi::millions::detail::run_experiment<digits, pi::millions::detail::cubic_borwein_for_pi>("Cubic Borwein");
   pi::millions::detail::run_experiment<digits, pi::millions::detail::quartic_borwein_for_pi>("Quartic Borwein");
   pi::millions::detail::run_experiment<digits, pi::millions::detail::quintic_borwein_for_pi>("Quintic Borwein");
   pi::millions::detail::run_experiment<digits, pi::millions::detail::nonic_borwein_for_pi>("Nonic Borwein");
   // pi::millions::detail::run_experiment<pi::millions::detail::chudnovsky_for_pi>("Chudnovsky pi");
   
   RUN_ALL_TIMED_EXPERIMENTS(float_type, pi::millions::detail::gauss_legendre_pi_unleashed);

   return 0;
}
