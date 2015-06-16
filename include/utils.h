//
// Created by Aliaksandr Dubrouski on 6/11/15.
//

#ifndef BINOMIAL_UTILS_H
#define BINOMIAL_UTILS_H
#include <cached_primes.h>
#include <math.h>
namespace utils {
/**
 * The function returns the vector of flags that is true for vector indices that are primes:
 * (Wikipedia)Input: an integer n > 1
 *
 * Let A be an array of Boolean values, indexed by integers 2 to n,
 *       initially all set to true.
 *
 * for i = 2, 3, 4, ..., not exceeding √n:
 * if A[i] is true:
 * for j = i2, i2+i, i2+2i, i2+3i, ..., not exceeding n :
 * A[j] := false
 *
 * Output: all i such that A[i] is true.
 * Optimization for calculating the primes can be done with precalculating the
 * table of primes to do a simple lookup instead of calculation;
 *
 * @param[in] n max number to search primes for
 * @param[inout] res resulting vector of primes
 */
    template<typename T>
    void primes_less_or_equal(T n, std::vector<T> &res) {
        //assert(n > 1 && n < USHRT_MAX);
        //if(n<100) {
            // array for flags indicating primality
            std::vector<short> helper(n + 1);
            std::for_each(helper.begin(), helper.end(), [](short &elem) { elem = 1; });
            // exclude non-prime using the Erathosthenes sieve starting from 2
            for (auto i = 2; i < sqrt(n); ++i)
                for (auto j = i * i; j < n && helper[i]; j += i)
                    helper[j] = 0;

            T i = 0;
            std::for_each(helper.begin(), helper.end(), [&res, &i](T elem) {
                if (elem) res.push_back(i);
                ++i;
            });
//        } else {
//            auto it = cached_primes_int16().begin();
//            std::vector<T>(cached_primes_int16().begin(),);
//        }

    }

/**
 * @brief function returns prime numbers [2..sqrt(n)]
 *
 * The function produces prime factors for the submitted number by trial division.
 * The process includes finding all primes in the range of [2..sqrt(n)] and may be
 * very time consuming. For repeated calculation the optimization may be to memorize
 * the primes between the calls in some table.
 *
 * @param[in] n number that shall be factorized
 * @param[inout] factors map to store factors found
 */
    template<typename T, typename E>
    void prime_factors(T n, std::map<T, E> &factors) {
        //assert(n<std::numeric_limits<unsigned short>::max());
        // std::cout<<"n ="<<n<<" limit= "<<std::numeric_limits<unsigned short>::max()<<std::endl;
        const auto p = cached_primes_int16();
        // do a trial division and store the 'exponent'
        for (auto i = 0; i < (unsigned int)sqrt(n) + 1; ++i) {
            if (p[i] * p[i] > n) break;
            while (p[i] > 1 && 0 == n % p[i]) {
                factors[p[i]] += 1;
                n /= p[i];
            }
        }
        if (n > 1)
            factors[n] += 1;
    }
/**
 * @brief function to do a benchmarking and print time used
 */
    template<typename F, typename... Args>
    void timing(F f, Args&&... args ) {
        auto start(std::chrono::system_clock::now());
        f(args...);
        auto end(std::chrono::system_clock::now());
        std::cout<<"timing: "<<(end-start).count()<<std::endl;
    };
}
#endif //BINOMIAL_UTILS_H
