#if 0

///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 1999-2020.
//  Distributed under the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// chapter10_08-001_pi_millions_with_boost.cpp

// This program can be used to compute millions of digits of pi.
// In fact, it has been used to compute more than one billion
// decimal digits of pi. Boost.Multiprecision is combined with
// GMP (or MPIR) in order to carry out the calculation of pi.

// This program requires inclusion of Boost.Multiprecision
// and linking with GMP (or MPIR on certain targets).
// At the moment, slight modifications are required in
// Boost.Multiprecision in order to handle the large digit
// count. In particular, when exceeding a few tens of millions
// of digits, the expression of maximum digit2 and certain digit
// constants in numeric_limits need to be extended to long long.

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <regex>
#include <sstream>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/gmp.hpp>

namespace pi { namespace millions { namespace detail {

/**
// \func template<typename float_type> const float_type& pi()
//
// \desc Compute pi using a quadratically convergent Gauss AGM,
//       in the Schoenhage variant. This is a well-known algorithm.
//       For a particularly clear description of this algorithm,
//       see "Algorithm 16.148" in Chapter 16 on page 236 from
//       the book "Pi Unleashed" by J. Arndt and C. Haenel
//       (Springer Verlag, Heidelberg, 2001).
//       The parameter print_progress (= true),
//       will print calculation progress to std::cout.
//       Book reference for "Pi Unleashed":
//       https://www.springer.com/de/book/9783642567353
//
*/
template <typename float_type>
const float_type& pi(const bool print_progress = false)
{
   static bool is_init;

   static float_type val_pi;

   if (!is_init)
   {
      is_init = true;

      const std::regex rx("^[^e]+[e0+-]+([0-9]+)$");

      std::match_results<std::string::const_iterator> mr;

      std::stringstream ss;
      ss.setf(std::ios::scientific);
      ss.precision(static_cast<std::streamsize>(4));

      float_type a(1.0F);
      float_type bB(0.5F);
      float_type s(0.5F);
      float_type t(0.375F);

      // This loop is designed to compute a maximum of a few billion
      // decimal digits of pi. The number of digits roughly doubles
      // with each iteration of the loop. After 20 iterations,
      // the precision is about 2.8 million decimal digits.
      // After 29 iterations, the precision reaches more than
      // one billion decimal digits.

      for (std::uint_least16_t k = UINT8_C(1); k < UINT8_C(64); ++k)
      {
         using std::sqrt;

         a += sqrt(bB);
         a /= 2U;
         val_pi = a;
         val_pi *= val_pi;
         bB = (val_pi - t);
         bB *= 2U;

         const float_type iterate_term((bB - val_pi) * (UINT64_C(1) << k));

         s += iterate_term;

         // Extract the base-10 order of magnitude
         // to estimate the base-10 digits in this
         // iteration.

         // Here, we produce a short printout
         // of the iteration term that is subsequently
         // parsed with a regular expression
         // for extracting the base-10 order.

         // Note: We are only extracting a few digits from iterate_term.
         // So piping it to a stringstream is not exorbitantly costly here.
         ss << iterate_term;

         const std::string str_iterate_term(ss.str());

         const bool is_match =
             std::regex_match(str_iterate_term, mr, rx);

         const std::uint64_t digits10_iterate =
             (is_match ? (std::max)(boost::lexical_cast<std::uint64_t>(mr[1U]), std::uint64_t(0U))
                       : UINT64_C(0));

         if (print_progress)
         {
            std::cout << "Base-10 digits of iteration "
                      << std::right
                      << std::setw(3)
                      << k
                      << ": "
                      << std::right
                      << std::setw(12)
                      << digits10_iterate
                      << '\n';
         }

         // Test the approximate base-10 digits
         // of this iteration term.

         // If we have attained at least half or more
         // of the total desired digits with this
         // iteration, the calculation is finished
         // because the change from the next iteration will be
         // insignificantly small.
         BOOST_CONSTEXPR_OR_CONST std::uint64_t digits10_iterate_goal =
             static_cast<std::uint64_t>((static_cast<std::uint64_t>(std::numeric_limits<float_type>::digits10) + 1LL) / 2LL) + 16LL;

         if (digits10_iterate > digits10_iterate_goal)
         {
            break;
         }

         t = val_pi;
         t += bB;
         t /= 4U;

         ss.str(std::string());
      }

      if (print_progress)
      {
         std::cout << "Iteration loop done, compute inverse" << '\n';
      }

      val_pi += bB;
      val_pi /= s;

      if (print_progress)
      {
         std::cout << "The pi calculation is done." << '\n';
      }
   }

   return val_pi;
}

template <typename float_type>
std::ostream& report_pi_timing(std::ostream& os, const float elapsed)
{
   return os << "=================================================" << '\n'
             << "Computed "
             << static_cast<std::uint64_t>(std::numeric_limits<float_type>::digits10 - 1)
             << " digits of pi.\n"
             << "Total computation time : "
             << std::fixed
             << std::setprecision(2)
             << elapsed
             << " seconds"
             << '\n'
             << "================================================="
             << '\n';
}

}}} // namespace pi::millions::detail

namespace pi { namespace millions {

template <typename float_type>
void print_pi(std::ostream& os)
{
   // Calculate the value of pi. When doing so, print the calculation
   // messages to the console. Use the clock function to obtain the
   // total time of the pi calculation.

   using local_time_point_type =
       std::chrono::high_resolution_clock::time_point;

   const local_time_point_type start = std::chrono::high_resolution_clock::now();
   detail::pi<float_type>(true);
   const local_time_point_type stop = std::chrono::high_resolution_clock::now();

   // Evaluate the time that was required for the pi calculation.
   const float elapsed =
       static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()) / static_cast<float>(1000.0F);

   // Report the time of the pi calculation to the console.
   static_cast<void>(detail::report_pi_timing<float_type>(std::cout, elapsed));

   // Report the time of the pi calculation to the output stream.
   static_cast<void>(detail::report_pi_timing<float_type>(os, elapsed));

   // Report that we are writing the output file.
   std::cout << "Writing the output file." << '\n';

   // Pipe the value of pi into a stringstream object.
   std::stringstream ss;

   // Pipe the value of pi into a stringstream object with full precision.
   ss << std::fixed
      << std::setprecision(std::streamsize(std::numeric_limits<float_type>::digits10) - 1)
      << detail::pi<float_type>();

   // Extract the string value of pi.
   const std::string str_pi(ss.str());

   // Print pi using the following paramater-tunable format.

   // pi = 3.1415926535 8979323846 2643383279 5028841971 6939937510 : 50
   //        5820974944 5923078164 0628620899 8628034825 3421170679 : 100
   //        8214808651 3282306647 0938446095 5058223172 5359408128 : 150
   //        4811174502 8410270193 8521105559 6446229489 5493038196 : 200
   //        ...

   BOOST_CONSTEXPR_OR_CONST char* char_set_separator   = " ";
   BOOST_CONSTEXPR_OR_CONST char* char_group_separator = "\n";

   BOOST_CONSTEXPR_OR_CONST std::size_t digits_per_set   = 10U;
   BOOST_CONSTEXPR_OR_CONST std::size_t digits_per_line  = digits_per_set * 10U;
   BOOST_CONSTEXPR_OR_CONST std::size_t digits_per_group = digits_per_line * 10U;

   constexpr char line_end_delimiter_for_digits[] = ", // : ";

   // The digits after the decimal point are grouped
   // in sets of digits_per_set with digits_per_line
   // digits per line. The running-digit count is reported
   // at the end of each line.

   // The char_set_separator character string is inserted
   // between sets of digits. Between groups of lines,
   // we insert a char_group_separator character string
   // (which likely might be selected as a newline).

   // For a simple verification of 1,000,000 digits,
   // for example, go to Wolfram Alpha and ask:
   //   1000000th digit of Pi.
   // This prints out 50 digits of pi in the neighborhood
   // of a million digits, with the millionth digit in bold.

   std::string::size_type pos;

   if (((pos = str_pi.find(char('3'), 0U)) != std::string::npos) && ((pos = str_pi.find(char('.'), 1U)) != std::string::npos) && ((pos = str_pi.find(char('1'), 1U)) != std::string::npos))
   {
      ;
   }
   else
   {
      pos = 0U;
   }

   os << "pi = " << str_pi.substr(0U, pos);

   const std::size_t digit_offset = pos;

   // Extract the digits after the decimal point in a loop.
   // Insert spaces, newlines and a running-digit count
   // in order to create a format for comfortable reading.

   bool all_output_streaming_is_finished = false;

   while (all_output_streaming_is_finished == false)
   {
      // Print a set of digits (i.e. having 10 digits per set).
      const std::string str_pi_substring(str_pi.substr(pos, digits_per_set));

      os << str_pi_substring << char_set_separator;

      pos += (std::min)(std::string::size_type(digits_per_set),
                        str_pi_substring.length());

      const std::size_t number_of_digits(pos - digit_offset);

      // Check if all output streaming is finished.
      all_output_streaming_is_finished = (pos >= str_pi.length());

      if (all_output_streaming_is_finished)
      {
         // Write the final digit count.
         // Break from the printing loop.
         // Flush the output stream with std::endl.

         os << line_end_delimiter_for_digits << number_of_digits << std::endl;
      }
      else
      {
         const bool this_line_is_finished =
             (std::size_t(number_of_digits % digits_per_line) == std::size_t(0U));

         if (this_line_is_finished)
         {
            // Print the running-digit count and start a new line.
            os << line_end_delimiter_for_digits << number_of_digits << std::endl;

            const bool this_group_of_lines_is_finished =
                (std::size_t(number_of_digits % digits_per_group) == std::size_t(0U));

            if (this_group_of_lines_is_finished)
            {
               // Insert a character (which might be a blank line)
               // after a group of lines.
               os << char_group_separator;
            }

            // Insert spaces at the start of the new line.
            os << "       ";
         }
      }
   }
}

}} // namespace pi::millions

const std::string kPiStr = "3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502841027019385211055596446229489549303819644288109756659334461284756482337867831652712019091456485669234603486104543266482133936072602491412737245870066063155881748815209209628292540917153643678925903600113305305488204665213841469519415116094330572703657595919530921861173819326117931051185480744623799627495673518857527248912279381830119491298336733624406566430860213949463952247371907021798609437027705392171762931767523846748184676694051320005681271452635608277857713427577896091736371787214684409012249534301465495853710507922796892589235420199561121290219608640344181598136297747713099605187072113499999983729780499510597317328160963185950244594553469083026425223082533446850352619311881710100031378387528865875332083814206171776691473035982534904287554687311595628638823537875937519577818577805321712268066130019278766111959092164201989380952572010654858632788659361533818279682303019520353018529689957736225994138912497217752834791315155748572424541506959508295331168617278558890750983817546374649393192550604009277016711390098488240128583616035637076601047101819429555961989467678374494482553797747268471040475346462080466842590694912933136770289891521047521620569660240580381501935112533824300355876402474964732639141992726042699227967823547816360093417216412199245863150302861829745557067498385054945885869269956909272107975093029553211653449872027559602364806654991198818347977535663698074265425278625518184175746728909777727938000816470600161452491921732172147723501414419735685481613611573525521334757418494684385233239073941433345477624168625189835694855620992192221842725502542568876717904946016534668049886272327917860857843838279679766814541009538837863609506800642251252051173929848960841284886269456042419652850222106611863067442786220391949450471237137869609563643719172874677646575739624138908658326459958133904780275900994657640789512694683983525957098258226205224894077267194782684826014769909026401363944374553050682034962524517493996514314298091906592509372216964615157098583874105978859597729754989301617539284681382686838689427741559918559252459539594310499725246808459872736446958486538367362226260991246080512438843904512441365497627807977156914359977001296160894416948685558484063534220722258284886481584560285060168427394522674676788952521385225499546667278239864565961163548862305774564980355936345681743241125150760694794510965960940252288797108931456691368672287489405601015033086179286809208747609178249385890097149096759852613655497818931297848216829989487226588048575640142704775551323796414515237462343645428584447952658678210511413547357395231134271661021359695362314429524849371871101457654035902799344037420073105785390621983874478084784896833214457138687519435064302184531910484810053706146806749192781911979399520614196634287544406437451237181921799983910159195618146751426912397489409071864942319615679452080951465502252316038819301420937621378559566389377870830390697920773467221825625996615014215030680384477345492026054146659252014974428507325186660021324340881907104863317346496514539057962685610055081066587969981635747363840525714591028970641401109712062804390397595156771577004203378699360072305587631763594218731251471205329281918261861258673215791984148488291644706095752706957220917567116722910981690915280173506712748583222871835209353965725121083579151369882091444210067510334671103141267111369908658516398315019701651511685171437657618351556508849099898599823873455283316355076479185358932261854896321329330898570642046752590709154814165498594616371802709819943099244889575712828905923233260972997120844335732654893823911932597463667305836041428138830320382490375898524374417029132765618093773444030707469211201913020330380197621101100449293215160842444859637669838952286847831235526582131449576857262433441893039686426243410773226978028073189154411010446823252716201052652272111660396665573092547110557853763466820653109896526918620564769312570586356620185581007293606598764861179104533488503461136576867532494416680396265797877185560845529654126654085306143444318586769751456614068007002378776591344017127494704205622305389945613140711270004078547332699390814546646458807972708266830634328587856983052358089330657574067954571637752542021149557615814002501262285941302164715509792592309907965473761255176567513575178296664547791745011299614890304639947132962107340437518957359614589019389713111790429782856475032031986915140287080859904801094121472213179476477726224142548545403321571853061422881375850430633217518297986622371721591607716692547487389866549494501146540628433663937900397692656721463853067360965712091807638327166416274888800786925602902284721040317211860820419000422966171196377921337575114959501566049631862947265473642523081770367515906735023507283540567040386743513622224771589150495309844489333096340878076932599397805419341447377441842631298608099888687413260472";

template <typename T>
T quadratic_borwein_for_pi(size_t correct_digits)
{
   size_t max_iter = 1 + std::log2(std::numeric_limits<T>::radix) * std::log(correct_digits) / std::log(2.0);
   T      two      = 2.0;
   T      sqrt_2   = sqrt(two);
   T      a_n = sqrt_2, b_n = 0.0, p_n = 2 + sqrt_2;
   for (size_t i = 0; i < max_iter; ++i)
   {
      T sqrt_a_n = sqrt(a_n);
      T new_a_n  = (sqrt_a_n + 1 / sqrt_a_n) / 2;
      T new_b_n  = (1 + b_n) * sqrt_a_n / (a_n + b_n);
      a_n        = new_a_n;
      b_n        = new_b_n;
      p_n        = (1 + a_n) * p_n * b_n / (1 + b_n);
   }
   return p_n;
}

template <typename T>
T cubic_borwein_for_pi(size_t correct_digits)
{
   size_t max_iter = 1 + std::log2(std::numeric_limits<T>::radix) * std::log(correct_digits) / std::log(2.0);
   T      three    = 3.0;
   T      a_n = 1.0 / three, s_n = (sqrt(three) - 1) / 2, pow_3 = 1;
   for (size_t i = 0; i < max_iter; ++i)
   {
      T r_n = 3 / (1 + 2 * cbrt(1 - cube(s_n)));
      s_n   = (r_n - 1) / 2;
      a_n   = sqr(r_n) * a_n - pow_3 * (sqr(r_n) - 1);
      pow_3 *= 3;
   }
   return 1 / a_n;
}

template <typename T>
T quartic_borwein_for_pi(size_t correct_digits)
{
   size_t max_iter = 1 + std::log2(std::numeric_limits<T>::radix) * std::log(correct_digits) / std::log(2.0);
   T      two      = 2.0;
   T      sqrt_2   = sqrt(two);
   T      y_n      = sqrt_2 - 1;
   T      a_n = 2 * sqr(y_n), p_n = 8;
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

template <typename T>
T quintic_borwein_for_pi(size_t correct_digits)
{
   size_t max_iter     = 1 + std::log2(std::numeric_limits<T>::radix) * std::log(correct_digits) / std::log(2.0);
   T      two          = 2.0;
   T      five         = 5.0;
   T      inverse_five = 1 / five;
   T      sqrt_5       = sqrt(five);
   T      a_n = 1 / two, s_n = 5 * (sqrt_5 - 2);
   T      pow_five = 1;
   for (size_t i = 0; i < max_iter; ++i)
   {
      T x_n = 5 / s_n - 1;
      T y_n = sqr(x_n - 1) + 7;
      T z_n = kth_root(x_n / 2 * (y_n + sqrt(sqr(y_n) - 4 * cube(x_n))), 5);
      // T z_n = pow(x_n / 2 * (y_n + sqrt(sqr(y_n) - 4 * cube(x_n))), inverse_five);
      a_n   = sqr(s_n) * a_n - pow_five * ((sqr(s_n) - 5) / 2 + sqrt(s_n * (s_n * s_n - 2 * s_n + 5)));
      s_n   = 25 / (sqr(z_n + x_n / z_n + 1) * s_n);
      pow_five *= 5;
   }
   return 1 / a_n;
}

template <typename T>
T nonic_borwein_for_pi(size_t correct_digits)
{
   size_t max_iter     = 3 + std::log2(std::numeric_limits<T>::radix) * std::log(correct_digits) / std::log(9.0);
   T      three        = 3.0;
   T      inv_three    = 1.0 / three;
   T      pow_of_three = inv_three;
   T      a_n = inv_three, r_n = (sqrt(three) - 1) / 2;
   T      s_n = cbrt(1 - r_n * r_n * r_n);

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

template <typename T>
T gauss_legendre_for_pi(size_t correct_digits)
{
   size_t max_iter = 3 + std::log2(std::numeric_limits<T>::radix) * std::log(correct_digits) / std::log(2.0);
   T      two      = 2.0;
   T      a_n = 1.0, b_n = 1.0 / sqrt(two), t_n = 0.25, p_n = 1.0;

   for (size_t i = 0; i < max_iter; ++i)
   {
      T new_a_n = (a_n + b_n) / 2.0;
      t_n       = t_n - p_n * sqr(new_a_n - a_n);
      b_n       = sqrt(a_n * b_n);
      a_n       = new_a_n;
      p_n *= 2.0;
   }

   return sqr(a_n + b_n) / (4 * t_n);
}

template <typename FloatType, typename FloatType f(size_t)>
void run_experiment(size_t correct_digits)
{
   FloatType pi_reference = static_cast<FloatType>(kPiStr);

   using local_time_point_type =
       std::chrono::high_resolution_clock::time_point;
   const local_time_point_type start     = std::chrono::high_resolution_clock::now();
   FloatType                   estimated = f(correct_digits);
   const local_time_point_type stop      = std::chrono::high_resolution_clock::now();

   const float elapsed =
       static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()) / static_cast<float>(1000.0F);

   std::cout << "  Digits : " << correct_digits << std::endl;
   std::cout << "    Computation time : " << elapsed << "s" << std::endl;
   FloatType relative_error = (estimated - pi_reference) / pi_reference;
   std::cout << "    Relative error   : " << std::setprecision(3) << relative_error << std::endl;
}

template <typename FloatType, typename FloatType f(size_t)>
void run_experiments_for_algorithm(const std::string& name)
{
   std::cout << "Testing : " << name << std::endl;
   run_experiment<FloatType, f>(5);
   run_experiment<FloatType, f>(100);
   run_experiment<FloatType, f>(200);
   run_experiment<FloatType, f>(1000);
   run_experiment<FloatType, f>(3000);
   run_experiment<FloatType, f>(5000);
   run_experiment<FloatType, f>(7000);
   run_experiment<FloatType, f>(9000);
}

template <typename FloatType>
void run_experiments_for_float_type(const std::string& float_type_name)
{
   std::cout << " ---- " << float_type_name << " ----" << std::endl;
   run_experiments_for_algorithm<FloatType, gauss_legendre_for_pi<FloatType> >("Guass-Legendre algorithm");
   run_experiments_for_algorithm<FloatType, quadratic_borwein_for_pi<FloatType> >("Quadratic Borwein");
   run_experiments_for_algorithm<FloatType, cubic_borwein_for_pi<FloatType> >("Cubic Borwein");
   run_experiments_for_algorithm<FloatType, quartic_borwein_for_pi<FloatType> >("Quartic Borwein");
   run_experiments_for_algorithm<FloatType, quintic_borwein_for_pi<FloatType> >("Quintic Borwein");
   run_experiments_for_algorithm<FloatType, nonic_borwein_for_pi<FloatType> >("Nonic Borwein");
   std::cout << std::endl
             << std::endl;
}

int main()
{
   using gmp_float_type =
       boost::multiprecision::number<boost::multiprecision::gmp_float<10000>,
                                     boost::multiprecision::et_off>;

   run_experiments_for_float_type<gmp_float_type>("gmp");

   using cpp_bin_float_type =
       boost::multiprecision::number<boost::multiprecision::cpp_bin_float<10000>,
                                     boost::multiprecision::et_off>;

   run_experiments_for_float_type<cpp_bin_float_type>("cpp_bin_10000");

   using cpp_dec_float_type =
       boost::multiprecision::number<boost::multiprecision::cpp_dec_float<10000>,
                                     boost::multiprecision::et_off>;

   run_experiments_for_float_type<cpp_dec_float_type>("cpp_dec_10000");
   /* boost::multiprecision::number<boost::multiprecision::cpp_bin_float<1000>,
                                 boost::multiprecision::et_off>
       val(12345678); 
   boost::multiprecision::number<boost::multiprecision::cpp_dec_float<1000>,
                                 boost::multiprecision::et_off>
       val(12345678); 
   // std::cout << std::setprecision(10) << kth_root(val, 3) << std::endl;
   std::cout << "kth root : " << std::setprecision(1000) << kth_root(val, 3) << std::endl; */
}

#endif

