///////////////////////////////////////////////////////////////////////////////
//      Copyright Christopher Kormanyos 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#define BOOST_TEST_MODULE test_uintwide_t
#include <boost/test/included/unit_test.hpp>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random.hpp>

#include <math/wide_integer/generic_template_uintwide_t.h>

template<typename UnsignedIntegralType>
static std::string hexlexical_cast(const UnsignedIntegralType& u)
{
  std::stringstream ss;

  ss << std::hex << u;

  return ss.str();
}

template<typename OtherLocalUintType,
         typename OtherBoostUintType>
static void get_equal_random_test_values_boost_and_local_n(OtherLocalUintType* u_local,
                                                           OtherBoostUintType* u_boost,
                                                           const std::size_t count)
{
  static_assert((std::numeric_limits<OtherLocalUintType>::digits == std::numeric_limits<OtherBoostUintType>::digits),
                "Error: Multiprecision integer types must have equal bit counts.");

  boost::mt11213b base_gen(std::clock());

  using independent_bits_local_uint_engine_type =
    boost::random::independent_bits_engine<boost::mt11213b, std::numeric_limits<OtherLocalUintType>::digits, OtherLocalUintType>;

  independent_bits_local_uint_engine_type gen_local_uint_type(base_gen);

  using other_local_uint_type = OtherLocalUintType;
  using other_boost_uint_type = OtherBoostUintType;

  for(std::size_t i = 0U; i < count; ++i)
  {
    const other_local_uint_type a = gen_local_uint_type();

    u_local[i] = a;
    u_boost[i] = other_boost_uint_type("0x" + hexlexical_cast(a));
  }
}

constexpr std::size_t digits2 = 1024UL << 3U;
constexpr std::size_t size() { return 1000U; }

using boost_uint_backend_type =
    boost::multiprecision::cpp_int_backend<digits2,
                                           digits2,
                                           boost::multiprecision::unsigned_magnitude>;

using boost_uint_type = boost::multiprecision::number<boost_uint_backend_type, boost::multiprecision::et_off>;

using local_limb_type = std::uint32_t;

using local_uint_type = wide_integer::generic_template::uintwide_t<digits2, local_limb_type>;

std::vector<local_uint_type> a_local;
std::vector<boost_uint_type> a_boost;
std::vector<local_uint_type> result_local;
std::vector<boost_uint_type> result_boost;

void initialize()
{
  a_local.clear();
  a_boost.clear();

  a_local.resize(size());
  a_boost.resize(size());

  result_local.clear();
  result_boost.clear();

  result_local.resize(size());
  result_boost.resize(size());

  get_equal_random_test_values_boost_and_local_n(a_local.data(), a_boost.data(), size());
}

BOOST_AUTO_TEST_CASE(test_uintwide_t_dummy_tag)
{
  initialize();

  const std::clock_t start_local = std::clock();
  for(std::size_t i = 0U; i < size(); ++i)
  {
    result_local[i] = sqrt(a_local[i]);
  }
  const std::clock_t elapsed_local = std::clock() - start_local;

  const std::clock_t start_boost = std::clock();
  for(std::size_t i = 0U; i < size(); ++i)
  {
    result_boost[i] = sqrt(a_boost[i]);
  }
  const std::clock_t elapsed_boost = std::clock() - start_boost;

  for(std::size_t i = 0U; i < size(); ++i)
  {
    const std::string str_boost = hexlexical_cast(result_boost[i]);
    const std::string str_local = hexlexical_cast(result_local[i]);

    BOOST_CHECK(str_boost == str_local);
  }

  std::cout << "elapsed_local: " << double(elapsed_local) / double(CLOCKS_PER_SEC) << std::endl;
  std::cout << "elapsed_boost: " << double(elapsed_boost) / double(CLOCKS_PER_SEC) << std::endl;
}

/*
Using Boost 1.73
Running 1 test case...
elapsed_local: 4.185
elapsed_boost: 10.412

*** No errors detected
*/

/*
Using Boost GSoC 2020 log_agm branch (C:\boost\boost_gsoc2020\multiprecision\include;)
Running 1 test case...
elapsed_local: 4.161
elapsed_boost: 0.458

*** No errors detected
*/
