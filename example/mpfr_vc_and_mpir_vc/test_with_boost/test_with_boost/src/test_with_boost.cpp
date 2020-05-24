#include <iostream>
#include <iomanip>
#include <limits>

#include <boost/math/special_functions/gamma.hpp>
#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/mpfr.hpp>

namespace test_with_boost
{
  using float_type_gmp  = boost::multiprecision::number<boost::multiprecision::gmp_float<1000U>,
                                                        boost::multiprecision::et_off>;

  using float_type_mpfr = boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<1000U>,
                                                       boost::multiprecision::et_off>;
}

int main()
{
  const test_with_boost::float_type_gmp  x_gmp  = test_with_boost::float_type_gmp (31) / 3;
  const test_with_boost::float_type_mpfr x_mpfr = test_with_boost::float_type_mpfr(31) / 3;

  const test_with_boost::float_type_gmp  g_gmp  = tgamma(x_gmp);
  const test_with_boost::float_type_mpfr g_mpfr = tgamma(x_mpfr);

  std::cout << std::setprecision(std::numeric_limits<test_with_boost::float_type_gmp>::digits10)
            << g_gmp
            << std::endl;

  std::cout << std::setprecision(std::numeric_limits<test_with_boost::float_type_mpfr>::digits10)
            << g_mpfr
            << std::endl;
}
