## Overview

This is the final report for my Google Summer of Coder 2020 project with Boost.Multiprecision.  The aim of the project was to make sure that core arithmetic functions are efficient for numbers of up to 10K digits. This encompassed:

* implementation, testing and benchmarking of the:
  * square root function (around 23.5x faster than the existing implementation), 
  * logarithm function (around 35.5x faster than the existing implementation),
  * k-th root function (around 340x faster than the power-based implementation for small (<256) integer values),
  * several algorithms for computing digits of <img src="https://render.githubusercontent.com/render/math?math=%5Cpi">
* testing the implementation of existing basic arithmetic operations

The implementation of square root and logarithm turned out to be efficient for up to thousands of digits. 

The code can be found [here](https://github.com/BoostGSoC20/multiprecision/). In case, the math does not display well, you can download this report here TODO.

## SQRT implementation

The square root function is one of the most commonly used mathematical functions either as a standalone function or as a component of a more complicated ones (see <img src="https://render.githubusercontent.com/render/math?math=%5Clog">and <img src="https://render.githubusercontent.com/render/math?math=%5Cpi">computations, below).

### Implementations

The existing implementation for floating point numbers converts the number to an integer and then perform the square root operation in the integer space. To recover the square root of the floating point number, the exponent needs to be adjusted. Hence, the descriptions of the algorithms below focus on computing the square root of an integer.  

**Existing implementation:** The current implementation of the sqrt function uses the binary method which operates in two phases:

1. Using binary search find the largest power of two <img src="https://render.githubusercontent.com/render/math?math=2%5Ei"> whose square is smaller than the given number.
2. Go through the bits <img src="https://render.githubusercontent.com/render/math?math=i-1"> to <img src="https://render.githubusercontent.com/render/math?math=0"> and decide whether it should be a <img src="https://render.githubusercontent.com/render/math?math=1"> or a <img src="https://render.githubusercontent.com/render/math?math=0">.

The time complexity of this approach is linear to the number of precision bits of the given number.

**Newton's method:** Newton's method is a iterative method which can be used to find a root of a (sufficiently smooth) function <img src="https://render.githubusercontent.com/render/math?math=f">:

1. Choose an initial point <img src="https://render.githubusercontent.com/render/math?math=x_0">.
2. Set <img src="https://render.githubusercontent.com/render/math?math=x_%7Bt%2B1%7D%20%3A%3D%20x_t%20-%20%5Cfrac%7Bf(x_t)%7D%7Bf'(x_t)%7D">.
3. Repeat step (2) until two iterates <img src="https://render.githubusercontent.com/render/math?math=x_t"> and <img src="https://render.githubusercontent.com/render/math?math=x_%7Bt%2B1%7D"> are sufficiently close (smaller than the epsilon precision of the representation).

By choosing <img src="https://render.githubusercontent.com/render/math?math=f(x)%20%3D%20x%5E2%20-%20a">, <img src="https://render.githubusercontent.com/render/math?math=f(x)"> has its only positive root at <img src="https://render.githubusercontent.com/render/math?math=x%20%3D%20%5Csqrt%7Ba%7D">. The Newton-Raphson iteration becomes:

<p align="center">

<img src="https://render.githubusercontent.com/render/math?math=x_%7Bt%2B1%7D%20%3A%3D%20%5Cfrac%7B1%7D%7B2%7D%20%5Cleft(x_t%20%2B%20%5Cfrac%7Ba%7D%7Bx_t%7D%20%5Cright)%20">

</p>

For quadratic convergence, the initial value has to be within twice of the square root. For a value <img src="https://render.githubusercontent.com/render/math?math=a%20%3D%20y%202%5Ee">, a good initial value is <img src="https://render.githubusercontent.com/render/math?math=x_0%20%3D%20y%202%5E%7Be%2F2%7D">, which can be calculated efficiently by right shifting the number.

The implementation for Newton Raphson's method can be found here (TODO: add link to the code).

In theory, for a good initial value, the convergence rate is quadratic. However, in practice, the algorithm has several calls to the division subroutine, making the execution slower.

**Karatsuba square root:**

The Karatsuba square root method is a method that recursively computes the square root for the upper half of the digits and uses the division algorithm (and the lower half of the digits) to compute the square root of the entire number. It is named like this due to its similarity with Karatsuba's multiplication algorithm that also splits the digits into four parts. The pseudocode for the algorithm (based on (Zimmerman 1998) and (Brent and Zimmerman 2010)) is given below:

<p align="center">

<img src="sqrt_karatsuba_pseudocode.PNG" width="650px">

</p>

The main implementation can be found here.

One part that is not covered in the algorithm descriptions is how to solve the base case. The problem here is that we will be left with four 32-bit limbs and there is no built in algorithm to compute the square root of 128-bit integers. Continuing the splitting at the bit level at non-fixed positions will be tedious and not very efficient. Instead, we see notice that we can handle all cases as one of the following four cases:

<p align="center">

<img src="sqrt_base_case_visualisation.svg" height="400px">

</p>

The second case relies on the fact that we can fix the rounding problems in double sqrt for 64-bit integers (see code here). The third case splits into 32-bit integers which means that the recursive call can be handled by Case 2. The fourth case splits into 64-bit integers which means that the recursive call can be handled by Case 3.

### Correctness tests

The following correctness tests (which can be found here: TODO) were run for each of the implementations:

* A number of randomly chosen values in<img src="https://render.githubusercontent.com/render/math?math=%5B1%2C%202%5E10000%5D">:
  * with some of which the response was compared to hardcoded values from Wolfram Alpha.
  * with some of which the response was verified using the following integer square root check: <img src="https://render.githubusercontent.com/render/math?math=x"> is the square of <img src="https://render.githubusercontent.com/render/math?math=a">iff <img src="https://render.githubusercontent.com/render/math?math=(x%2B1)%5E2%20%3E%20a"> and <img src="https://render.githubusercontent.com/render/math?math=x%5E2%20%5Cleq%20a">. 
* A number of edge cases (containing <img src="https://render.githubusercontent.com/render/math?math=0">, <img src="https://render.githubusercontent.com/render/math?math=1">, powers of two)
* A number of cases that are difficult for specific algorithms: 
  * powers of two
  * powers of two minus one
  * numbers with different 1, 2, 3 or 4 limbs.
  * all possible squares of numbers in <img src="https://render.githubusercontent.com/render/math?math=%5B1%2C%202%5E32%5D"> (must enable `EXHAUSTIVE_TESTS`)
  * <img src="https://render.githubusercontent.com/render/math?math=x%5E2%20%2B%202x">for all numbers in <img src="https://render.githubusercontent.com/render/math?math=%5B1%2C%202%5E32%5D"> (these trigger the greatest possible remainder)

### Performance tests

The tests where performed on the same <img src="https://render.githubusercontent.com/render/math?math=20">randomly sampled numbers. The error bars represent confidence intervals at 95% assuming each trial is i.i.d.

<p align="center">

<img src="sqrt_benchmark.svg" height="300px">

</p>

As an extension we show that Karatsuba square root is efficient for up to 100K (and even more).

<p align="center">

<img src="sqrt_karatsuba.svg" heigh="300px">

</p>

## k-th root implementation

When Some of the <img src="https://render.githubusercontent.com/render/math?math=%5Cpi">algorithms require taking the fourth or fifth root of a number. The general case for this is taking the <img src="https://render.githubusercontent.com/render/math?math=k">-th integer root of a number <img src="https://render.githubusercontent.com/render/math?math=x">. The current way of doing this in Boost.Multiprecision is through <img src="https://render.githubusercontent.com/render/math?math=%5Cmathrm%7Bpow%7D(x%2C%201%2Fk)"> which is not very efficient for small <img src="https://render.githubusercontent.com/render/math?math=k">.

**Newton-Raphson method:** Similarly to the Newton-Raphson implementation of the square root, we define<img src="https://render.githubusercontent.com/render/math?math=f(x)%20%3D%20x%5Ek%20-%20a">, whose derivative is<img src="https://render.githubusercontent.com/render/math?math=f'(x)%20%3D%20(k-1)x">and which gives rise to the iteration method

<p align="center">
   <img src="https://render.githubusercontent.com/render/math?math=x_%7Bt%2B1%7D%20%3A%3D%20%5Cfrac%7B1%7D%7Bk%7D%20%5Cleft(%20(k-1)%20x_t%20-%20a%20%5Cright)"> 
</p>

### Performance tests

Comparing with the existing implementation, the new implementation is 340x faster for 10K digits,

<p align="center" >
    <img src="kth_root_comparison.svg" height="300px">
</p>

See below the performance for various values of k,

<p align="center" >
    <img src="kth_root_for_various_k.svg" height="300px">
</p>

As an extension, the performance tests for up to 100K digits, show that the implementation is efficient for an even wider range of values.

<p align="center" >
    <img src="kth_root_large_values.svg" height="300px">
</p>

## PI algorithms

### Implementations

**Existing implementation:** The existing implementation simply has a hardcoded value for pi. 



### Correctness tests

The tests simply compared the digits with various publicly available collections of pi digits.

### Performance tests



## Log implementation

### Implementations

**Existing implementation:** TODO

**AGM-based:** Gauss introduced the arithmetic-geometric method, where two initial values <img src="https://render.githubusercontent.com/render/math?math=a_0"> and <img src="https://render.githubusercontent.com/render/math?math=b_0"> are chosen and 

<p align="center">

<img src="https://render.githubusercontent.com/render/math?math=a_n%20%3A%3D%20%5Cfrac%7Ba_%7Bn-1%7D%20%2B%20b_%7Bn-1%7D%7D%7B2%7D%20%5Cquad%20%5Ctext%7B%20and%20%7D%20%5Cquad%20b_n%20%3A%3D%20%5Csqrt%7Ba_%7Bn-1%7D%20b_%7Bn-1%7D%7D.">

</p>

For any initial values with <img src="https://render.githubusercontent.com/render/math?math=a_0%20%5Cgeq%20b_0">, the sequence converges to a finite value <img src="https://render.githubusercontent.com/render/math?math=M(a_0%2C%20b_0)">by Bolzanno-Weirstrass as <img src="https://render.githubusercontent.com/render/math?math=a_0%20%5Cleq%20a_1%20%5Cleq%20%5Cldots%20%5Cleq%20b_1%20%5Cleq%20b_0">. 

Gauss proved that a sequence with <img src="https://render.githubusercontent.com/render/math?math=a_0%20%3D%201"> and <img src="https://render.githubusercontent.com/render/math?math=b_0%20%3D%20x"> converges to <img src="https://render.githubusercontent.com/render/math?math=%5Cfrac%7B%5Cpi%7D%7B2E(x)%7D">, where 

<p align="center">

<img src="https://render.githubusercontent.com/render/math?math=E(x)%20%3D%20%5Cint_0%5E%7B%5Cpi%2F2%7D%20%5Cfrac%7Bd%5Ctheta%7D%7B%5Csqrt%7B1%20-%20(1%20-%20x%5E2)%5Csin%5E2%20%5Ctheta%7D%7Ddx">

</p>

The elliptic function <img src="https://render.githubusercontent.com/render/math?math=E(x)"> satisfies <img src="https://render.githubusercontent.com/render/math?math=E(4%2Fx)%20%3D%20%5Cln(x)%20%2B%20%5Cfrac%7B4%5Cln(x)-4%7D%7Bx%5E2%7D%20%2B%20o(x%5E%7B-2%7D)">. Hence, one can evaluate <img src="https://render.githubusercontent.com/render/math?math=x"> to <img src="https://render.githubusercontent.com/render/math?math=p"> digits of precision using

<p align="center">

<img src="https://render.githubusercontent.com/render/math?math=%5Cln(x)%20%5Capprox%20%5Cfrac%7B%5Cpi%7D%7B2M(1%2C%204%2Fs)%7D%20-%20m%20%5Cln(2)">

</p>

where <img src="https://render.githubusercontent.com/render/math?math=s%20%3D%20x2%5Em%20%3E%202%5E%7Bp%2F2%7D">. This algorithm has <img src="https://render.githubusercontent.com/render/math?math=%5Cmathcal%7BO%7D(%5Clog%20p)"> operational complexity and <img src="https://render.githubusercontent.com/render/math?math=%5Cmathcal%7BO%7D(M(p)%20%5Clog%20p)">, where <img src="https://render.githubusercontent.com/render/math?math=M(p)"> is the time complexity of multiplication.

### Performance tests

The tests where performed on the same <img src="https://render.githubusercontent.com/render/math?math=20">randomly sampled numbers. The error bars represent confidence intervals at 95% assuming each trial is i.i.d.

<p align="center">

<img src="log_timings.svg" heigh="300px">

</p>

For more emphasis on the newly implemented method using the Karatsuba sqrt function:

<p align="center">

<img src="log_agm_timings.svg" height="300px">

</p>

As an extension we show that the log AGM implementation performance for up to 100K (and even more).

<p align="center">

<p align="center">
    <img src="log_agm_large.svg" height="300px">
</p>





## VC builds for MPFR and MPIR

TODO



## References

Brent, Richard P., and Zimmermann, Paul. "Modern computer arithmetic". Vol. 18. Cambridge University Press, 2010, pp.26

Zimmermann, Paul. "Karatsuba Square Root". [Research Report] RR-3805, INRIA. 1999, pp.8

Future work:

* The following functions could be optimised further: eval_msb, create a function that computes both the remainder and the quotient.