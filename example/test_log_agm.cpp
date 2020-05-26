#include <chrono>
#include <limits>
#include <iomanip>
#include <iostream>

#include <boost/math/constants/constants.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>

namespace boost_gsoc2020
{
  template <class clock_type>
  struct stopwatch
  {
  public:
    typedef typename clock_type::duration duration_type;

    stopwatch() : m_start(clock_type::now()) { }

    stopwatch(const stopwatch& other) : m_start(other.m_start) { }

    stopwatch& operator=(const stopwatch& other)
    {
      m_start = other.m_start;
      return *this;
    }

    ~stopwatch() { }

    duration_type elapsed() const
    {
      return (clock_type::now() - m_start);
    }

    void reset()
    {
      m_start = clock_type::now();
    }

  private:
    typename clock_type::time_point m_start;
  };

  using big_float_type = boost::multiprecision::number<boost::multiprecision::cpp_bin_float<1000>,
                                                       boost::multiprecision::et_off>;

  using stopwatch_type = stopwatch<std::chrono::high_resolution_clock>;

  extern const std::string str_log_ctrl;
}

int main()
{
  // N[Log[(1/10) + (Pi 10000)], 1000]

  const boost_gsoc2020::big_float_type dummy_ln2 = boost::math::constants::ln_two<boost_gsoc2020::big_float_type>();

  boost_gsoc2020::stopwatch_type my_stopwatch;

  //boost_gsoc2020::big_float_type x = 10 + boost::math::constants::third<boost_gsoc2020::big_float_type>();
  boost_gsoc2020::big_float_type x =   (boost_gsoc2020::big_float_type(1) / 10)
                                     + (boost::math::constants::pi<boost_gsoc2020::big_float_type>() * 10000U);

  // Compare with N[Log[31/3], 10000]
  my_stopwatch.reset();

  const boost_gsoc2020::big_float_type log_value = log(x);

  const auto execution_time =
    std::chrono::duration_cast<std::chrono::duration<float>>(my_stopwatch.elapsed()).count();

  const boost_gsoc2020::big_float_type log_cntrl { boost_gsoc2020::str_log_ctrl };

  std::cout << std::setprecision(std::numeric_limits<boost_gsoc2020::big_float_type>::digits10)
            << log_value
            << std::endl;

  std::cout << "execution_time: " << std::setprecision(3) << execution_time << "s" << std::endl;

  const boost_gsoc2020::big_float_type closeness = fabs(1 - fabs(log_value / log_cntrl));

  std::cout << "closeness: " << std::setprecision(3) << closeness << std::endl;
}

const std::string boost_gsoc2020::str_log_ctrl
{
  "10."
  "35507344091937869969047747203647046931579490537265046111952055740596625019155494838884779430692370612"
  "83731527802349886384439548213279535430320752288823068939015400381565032654169509325492081747136408406"
  "70002321534598356631953006304821201484830902632165858733357596733427094375150860310219829690440417223"
  "99288553356638811859524852319374077805289364727170061213667011422428037421162678710607869364328911230"
  "14681999175321061380699515337395196318408715616104362458912133401155160492620400406291100670441895211"
  "95424265893260820047241700547626152805806168376431593536142731444390115903049252763147757606956640085"
  "35012766745322953474038055704899204102382957147868255773259362983129564730038256827794745671966940090"
  "48756947614076621843282480059847838183517228659200094881054410948493864962453511380422730814287511954"
  "48721675316518469911620672446127967634101401652522987085642801553766217253388437025836969828343977624"
  "54628045726208467029067040390334987220046238123964305899163204860505853753158222674220987870274493379"
};
