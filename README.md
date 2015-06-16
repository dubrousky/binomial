# Binomial calculation using C++11 threading capabilities

## Contents
Calculation is based on the formula 
```
C(n,k) = n!/(k!(n-k)!) = P(k+1,..,n)/P(1,..k)
```
where ```P``` stands for product. There are two approaches that were implemented:

* straightforward approach to parallelize the direct multiplication (std::accumulate) - requires big int types for performing operations (n>19).
* factorization of each multiplier into primes and summing up the powers of primes - makes the unsigned short enough for calculation but requires big int for result (n>100)

## Contents

binomials.h contains the front-end to the following algorithms:
```c++
binomials<T,calculate::mult_seq>
```
implements the calculation of binomials using direct sequential multiplication
```c++
binomials<T,calculate::mult_async>
```
implements the calculation of binomials using direct parallel multiplication
```c++
binomials<T,calculate::factorize_seq>
```
implements the calculation of binomials using sequential factorization
```c++
binomials<T,calculate::factorize_async>
```
implements the calculation of binomials using parallel factorization

## Parallelization
The parallel version of algorithms uses std::async to perform multiplication/factorization subtasks based on the fork_threshold that tells how many elements the subtask needs to process.

## Drawbacks
* The parallel version uses std::async that may spawn the background threads as needed introducing the additional cost on thread creation.
* The calculation of denominator and numerator are sequential: denominator is calculated first to see if is valid since it may be 0 due to the overflows. One may add async calculation of boths.

## TODO:

* Parameterize the algorithms with calculation strategies based on the execution context (std::async, thread_pool, tbb::parallel_for, boost_thread_group, etc.)
* Calculate the denominator asynchronously
* Exception handling: add mechanism to cancel ongoing calculation if one of the subtasks fails in order not to wait until the calculation completes.



