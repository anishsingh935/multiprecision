#include <iostream>
#include <iomanip>
#include <limits>

#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/mpfr.hpp>

namespace test_with_boost {

template <unsigned int Digits>
using float_type_gmp = boost::multiprecision::number<boost::multiprecision::gmp_float<Digits>,
                                                     boost::multiprecision::et_off>;

template <unsigned int Digits>
using float_type_mpfr = boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<Digits>,
                                                      boost::multiprecision::et_off>;

template<unsigned int Digits>
using float_cpp_bin = boost::multiprecision::number<boost::multiprecision::cpp_bin_float<Digits>, boost::multiprecision::et_off >;

template <unsigned int Digits>
using float_cpp_dec = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<Digits>, boost::multiprecision::et_off >;

template<typename T>
void test_simple_add(const std::string& type_name) {
  T a(132), b(274), ans(132 + 274);

  if (a + b == ans) {
    std::cout << type_name << " is OK." << std::endl << std::endl;
  } else {
    std::cout << type_name << " is NOT OK!" << std::endl << std::endl;
  }
}

}


int main()
{
   test_with_boost::test_simple_add<test_with_boost::float_type_gmp<1000> >("gmp");

   test_with_boost::test_simple_add<test_with_boost::float_type_mpfr<1000> >("mpfr");

   test_with_boost::test_simple_add<test_with_boost::float_cpp_dec<1000> >("cpp_dec_float<1000>");

   test_with_boost::test_simple_add<test_with_boost::float_cpp_bin<1000> >("cpp_bin_float<1000>");

   return 0;
}
