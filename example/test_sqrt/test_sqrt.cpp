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

#define BOOST_TEST_MODULE test_sqrt
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

constexpr std::size_t digits2() { return 1UL << 13U; } // Dimitris, Change to 14U to see first failures.
constexpr std::size_t size   () { return 1000U; }

using boost_uint_backend_type =
    boost::multiprecision::cpp_int_backend<digits2(),
                                           digits2(),
                                           boost::multiprecision::unsigned_magnitude>;

using boost_uint_type = boost::multiprecision::number<boost_uint_backend_type, boost::multiprecision::et_off>;

using local_limb_type = std::uint32_t;

using local_uint_type = wide_integer::generic_template::uintwide_t<digits2(), local_limb_type>;

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

BOOST_AUTO_TEST_CASE(test_square_root_single_trial)
{
  const char str_x[] =
  "0x"
  "48d4976f128f1c7bbcb9e9cafff3d0786de06db84a25fef6458c4c0612dd59d56a9294b39d4af936a7e19680cac82a237f435e8e0548dc1ce43d0420c945dce2"
  "1e9ca35d81d681c26057fd75fffb4b215ed57406fec144ffe658426aa1fa7bac1d0eab584382c21210d81819e67f126eec72e8423bd5db108568a14e7e2540bf"
  "6c79c8508421679f6e185a84bfbc5c8be256b15487c510af3288a764a37f80c00c93f555cc5b0b9df03b855fdff443095adfcaca41240e7575446f5ee843c00f"
  "dd4ea17771ec9e8321d7a1dd209beeb132c2677a81292d2c6a2f15cc4f5d573fa5634ffa4b93b6390ff130f9323153d3566914d587e54824c9f5d108f013614e"
  "4dd22f1669c6e54ff51fca1f8ecac9c704df6f49b7f54cea590b518b3afc768e5a776938f8ae2adaf8a7e047be9eb16f1d2f37bc3cf0c35c86d540db40e286d1"
  "1af6e1e09913cb60d9a3602122eef3acff4b1b9978c5f5f9d599d7a4cfffdc06ec680694dd7cb3467fda12ab83ac0d5573b4adcd4a2425f857f52521ef21624a"
  "48b5bdbebcd9a88f3aa3cd1936463bb191eda3fdda72c083dabc053c388892ac01a28eaceb94bf5ae82b2ffcec3df180e931608cd3d5b1e53c34c5e9126ff517"
  "f47dc1ef47d7c595486dac2c772b0ce2d6f02becceed913878aaabdeb71e3e8e91df3a351c8fd7100301c6b1ac707a9f6e35ea1e8323e5e739b7225d99374ca2"
  "7fcd4ce5068f609198654117ae2f2a29bb16cc050817f82a41403a9616e0e43275658e3f5c4c0ff9666b43fe73ba9b580a306c4776ae94f5f3e57569bb0a6765"
  "1f8f8c407097b1edb7efb769cd490f456492e92a047e9cc624faf25d772c7f3e9eedf1a2813fa7c5c33c8379aca5e060f260e9d8a56eff7146ffbec07d75cbf8"
  "c2d4697104fa562e409ed53b80cc82f94b8ed69732b1540adee36a629ff817a27b436de9c1d452dfe0073768cbc34ea05784b5d37166a03a30991b65553e065e"
  "dadaf98e193fefc34ed02b5e1e0029c5a47037a79f93a40c23ea838df85be6ed3725abd4e1403ff0f1b21515a08f44cb2b9c70aaf49612e0e1add7f6ec481c08"
  "57869d30148d0be85a9a8ed73292578e39507f77a690c158b54e0e39789a989fff31104957c14f177ea8eddecb4e62685e0ce3ce4f85e9b0e2157940fe538274"
  "f8be356929cff9a39bd2d0605344f4dbdfae218e8306442a50ab091d0e1d2e51549787bf8ae5d014abb442928f5b1881c8a29973e407d54318f69510a29a1862"
  "1fb8ce537330e58202479458e4d7e7e7cebec8a1364c26a0b290ceca25bb25ced9cef3e5c48ad7859bc5e14b45b0649cf9503467b6f25d62e390d47153cc0a44"
  "d4c452ae025feb98571fa0559890d558867f3298831ab261102ce31362a653fc33f4d039df7d73f196556b3725108d92b09c0675c2608b2b505d2e26ac09c994"
  "bce5dbcf48afe32e803238b48341b7fd6c0f13f35f1049b82cf7784a51b9a8d82196483e10a985409df2c0720837aa96f25c2cc4e17391d4dac037f4152c6901"
  "f73eb358cfb296f1ee027498139469da2dbe344806b5826267d839dd46a5ec4656a0dbbc00fa8acc6ff8123a4e3e2adca0bedc360fd3a8873ba68d0e2bce77ac"
  "49f47749ef8d710336971cb9e2b214aaa27dc09b1640fcc803a31e23d0abb9a8f84ab4fbf5ff3c58b531b3ff34662b5d09b5a8f363c0af066fade3db0b4c5aea"
  "5b8712e9c2f9fdf382fa82c6c8a551a24a8eaeb61442c6499d4297c147399fac3d80fa995d7b6c9bb291cfbab06e2ff5acec47c35172872e4b91360a8cf5a4e0"
  "602deeafa759ae8744f79b7c606b08295feb82f8c87329d6df8eccea12244baf3f8c114f2dcd64bdcf45d1351588fff69db5424a83739fa6df25a15f76c2bd86"
  "f668b97b2275d7f2ce7386962de058c3b252562553ec39882e1a805ea0a6ef745cebff32d083f03e85f63dfaec799a9cfbe1260af70b529b87d5c5c1e4cc972f"
  "7fe83adc4f5f9e330bb0fbca053d402919dfa4102e66adabaaf1bcfd9c85f4cb5c290e6a641cd86bab9fa09420021851502d7b7b63a1aa5bb8c9b65b965e1c17"
  "768b878feaaf7501a5166a2725237a8ff3cad50fe898147459c2af0645ea99d8c07438c728417d693070100f72ff975b0c4acf862050dd9a651ff5f36eed2aec"
  "41a5dd20f8ab3b3b61925ee926f364a684046fdd49be21dfa589b81405ed98f167a7281071bc3cd7da3f6b13e208b74db66576781119e24d4e6ca135268611c0"
  "186ba1a18206bc714109ee72ca9d8b08da406525e3ff9f315815703b4b312473adeaf2770ccb5dd61080b4192883426e1eadb1768c09a7bc15b25589daacc5a2"
  "c5b068dc994a45d577dab8a41233a601a8f6ad4db856eebf14f0f5fc076dd0dfb11404439b1f47461844aaaac18c674e08177b68bd080dcfd07094ebb4591b48"
  "b82720400f9311dd0084b8f1f65bc2abc95abcfb3fda58b771fbbf1c473358e1c9920f9fd7946a096a7473baa3cebb890117801addb3e7d9d30d8660705eeddf"
  "65a15ab3e57cad496d996ec6df3d71d391b9b76c402f505b8bb600c233cd80ac13ec17788c1480742053ee733fa57980cb22d10138b31ea7823bb53ec772f0cd"
  "e3e9bbdc64ff3d0126c0a0ce6a484ee6ddbfe63f78824dbfbaf25ef0486794f357d3ce2fa7fa4209d6d69618671d9d4d21043ce82c0663f9e1d6f1e7e284a8e5"
  "ba752e9463ade3c69c53da432c1e9befbfda231100b06911dfb13be87fd20cb6baa5d2782408ede0e160887b2701e7931a0339ef4f92112ea1e521f344410de9"
  "d4d7e91d45429b7a0e415216b5c8ba12a49cfbc5dfd5a2c7a2095d43c0757d3c68e3368b21a5a08641fe2e9ca674c0fd59156353d95ba48510fc0ebe7eee55af"
  ;

  const char str_y[] =
  "0x"
  "888b9233beefb8642ce471559c6b5d07f0103c3b77b441e8c2030bc4f2f51640e66e3a5f2a89a0c35035b1938a5717ee42bf6f5e538f22cc0ff46b757c1dc37f"
  "9640b4904cb81c8da3ac7c6897f10cc537304f1ff38db8c0681e27f8f48f7496a4f814778464f83c7fa0d3fe4c898088854a6e7f1943f26af15946330d0ef98d"
  "5ff35caffbe6710dd85bc2c9d7e6c3e33b004e96cace8d8f2bae5fb89258c4c359d4460b93566ebd1b8c3b3bc3490170398c96d84471042b33143bef1a8f1574"
  "6169625f1a45cb2d89499560d23628a2828716f64666f53b59664f90b9482e1873ee1c0b49bbc239c9cabf618b6c46bc41ead68eb0dcad012295fcea2605b8df"
  "6c5e3e5c9c9033e6a39d22b76f8ccf4f61c1ba39c35135f6780e9c4afebc6e49113d2b39e49dfbf125cc069c536b322a68d9380ad4de899810c20e7921e87cec"
  "6c309df200faacdbe7c93ba1f9bb6cd50e04774171ee23898a081a4c5d3b61092be2b4945e249fde455566621ad7d881fc3e966b764f55ea35e58c636c44bea6"
  "a6ae6385e0a79899ba06c81789fb4af713bc57e53225d5296acd355b57afbb7d45d2cb88f0f0e1cfa510e624fe5a3fa4a5c2fe9ba9ee75240a7baa0f79d1d437"
  "984d5b9c33d522388b62859819b453419bf7d28a5e89ec215a7f334644d8aac1010f2cb6e199a5d63945fd6848aabc129ef590a61542de9c71b73821dc08f8e0"
  "7f2cfb7c730aa43b92c815f6777386670a3526681db1fc53e4ba11cc4542cc42381e8a14cdc4954a738e77dc54330552240805ec4a413781bf7d5fbd6087477b"
  "4981687622822c8a47f3c685526c6f2fad97150b6ca54a6410dd35b23d9e5e3ac43d63aac250b2800f59713bed76be40a3e1bf4770722d62b3bfc17db6426d0e"
  "ca0e2fc31ee888c53f43b491d3d48b95f988eb35a5ac8de77ce555ce48eaac15468acae35e3f999d2c3e8363f5408d0cb009805d5ea96080ada33910ea2d0c33"
  "bac25ddeca9a594a474a308d4adc116c74b35b31edf58f454ba906ac67c40603f96cbbaeef3050be85d256e919ec00aea46a73d472f8ae8d58902a928cbcde90"
  "50cb2ac53df08ed9214af906cbd26f6b0110f9c8aac319ce04583d1c09f3525eae2c4d98c9b5fc5e3cb703405e364c6198d55f81c0ee6551a4537d99a8d69ccf"
  "0850c0be839670cee54e733a83b6061bb85c59229538978254223b8f7f02296bde27ad0582112ddd37a2172ca777fa296c985b22fd2af097b71ac8f16d874b8a"
  "d3d62d190cd745a5c64858236f42ad7c2b0bad8d323649714b5a70fa789155011bbd3eefdeec03cd628652b0b8e4c9123154c9e72545dcbb4896f785617baf4d"
  "df060b580f7d385b35873a8c06c5222560d99f7557ed4459a174713d99c35e68479f8fe68c0a8b2bf61e3ce9bf780921f77d059fb59f11a129de0d6b4440aba0"
  ;

  const boost_uint_type x_boost(str_x);
  const boost_uint_type y_boost = sqrt(x_boost);
  const local_uint_type x_local(str_x);
  const local_uint_type y_local = sqrt(x_local);

  const std::string str_control_boost = hexlexical_cast(y_boost);
  const std::string str_control_local = hexlexical_cast(y_local);

  BOOST_CHECK(str_control_boost == str_control_local);
}

BOOST_AUTO_TEST_CASE(test_square_root_random_trials)
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

  std::size_t number_of_trials_that_are_ok = 0U;

  for(std::size_t i = 0U; i < size(); ++i)
  {
    const std::string str_boost = hexlexical_cast(result_boost[i]);
    const std::string str_local = hexlexical_cast(result_local[i]);

    BOOST_CHECK(str_boost == str_local);
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

  std::cout << "elapsed_local: " << std::dec << double(elapsed_local) / double(CLOCKS_PER_SEC) << std::endl;
  std::cout << "elapsed_boost: " << std::dec << double(elapsed_boost) / double(CLOCKS_PER_SEC) << std::endl;
}
