///////////////////////////////////////////////////////////////////////////////
//      Copyright Christopher Kormanyos 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#define BOOST_TEST_MODULE test_sqrt
#include <boost/test/included/unit_test.hpp>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random.hpp>

#include <math/wide_integer/generic_template_uintwide_t.h>
namespace
{
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
    using other_local_uint_type = OtherLocalUintType;
    using other_boost_uint_type = OtherBoostUintType;

    using independent_bits_local_uint_engine_type =
      boost::random::independent_bits_engine<boost::mt11213b,
                                             std::numeric_limits<other_local_uint_type>::digits,
                                             other_local_uint_type>;

    boost::mt11213b base_gen(std::clock());

    independent_bits_local_uint_engine_type gen_local_uint_type(base_gen);

    for(std::size_t i = 0U; i < count; ++i)
    {
      const other_local_uint_type a = gen_local_uint_type();

      u_local[i] = a;
      u_boost[i] = other_boost_uint_type("0x" + hexlexical_cast(a));
    }
  }

  constexpr std::size_t digits2() { return 1UL << 14U; }
  constexpr std::size_t size   () { return 1000U; }
}

BOOST_AUTO_TEST_CASE(test_square_root_random_trials)
{
  using boost_uint_backend_type =
      boost::multiprecision::cpp_int_backend<digits2()>;

  using boost_uint_type =
    boost::multiprecision::number<boost_uint_backend_type, boost::multiprecision::et_off>;

  using local_limb_type = std::uint32_t;

  using local_uint_type = wide_integer::generic_template::uintwide_t<digits2(), local_limb_type>;

  std::vector<local_uint_type> a_local     (size());
  std::vector<local_uint_type> result_local(size());

  std::vector<boost_uint_type> a_boost     (size());
  std::vector<boost_uint_type> result_boost(size());

  get_equal_random_test_values_boost_and_local_n(a_local.data(), a_boost.data(), size());

  const std::clock_t start_local = std::clock();
  std::transform(a_local.cbegin(),
                 a_local.cend(),
                 result_local.begin(),
                 [](const local_uint_type& a) -> local_uint_type
                 {
                   return sqrt(a);
                 });
  const double elapsed_local_s = double(std::clock() - start_local) / double(CLOCKS_PER_SEC);

  const std::clock_t start_boost = std::clock();
  std::transform(a_boost.cbegin(),
                 a_boost.cend(),
                 result_boost.begin(),
                 [](const boost_uint_type& a) -> boost_uint_type
                 {
                   return sqrt(a);
                 });
  const double elapsed_boost_s = double(std::clock() - start_boost) / double(CLOCKS_PER_SEC);

  for(std::size_t i = 0U; i < size(); ++i)
  {
    const std::string str_boost = hexlexical_cast(result_boost[i]);
    const std::string str_local = hexlexical_cast(result_local[i]);

    BOOST_CHECK(str_boost == str_local);
  }

  std::cout << "elapsed_local_s: " << std::dec << elapsed_local_s << std::endl;
  std::cout << "elapsed_boost_s: " << std::dec << elapsed_boost_s << std::endl;
}
