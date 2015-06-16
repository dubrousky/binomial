//
// Created by Aliaksandr Dubrouski on 6/11/15.
//
#ifndef BINOMIAL_BUNOMIAL_FACTORIZATION_H
#define BINOMIAL_BUNOMIAL_FACTORIZATION_H
#include<paccumulate.h>
#include<factorization.h>
/**
 * @class template <typename T> class binomial_factorization
 * @brief calculates C(n,k) based on the factorization performed concurrently
 *
 * decomposition of product of numbers into the product of prime powers. the implementation shall use the
 * the sieve Eratosthenes of to factorize the numbers into primes:
 * find all primes in the range (1..sqrt(n)), decompose each multiplier in the numerator and denominator in
 * C(n,k) = P(k+1,...,n)/P(1,..,n-k), where P stands for product.
 * The calculation is performed as follows: an array of primes from 1 to sqrt(n) is created. The decomposition
 * into primes is calculated concurrently: the thread pool is created and each worker thread takes a number
 * and calculates its decomposition into primes; the result of calculation is submitted into the result array of prime
 *factor powers.
 */
template <typename T> class binomial_factorization : public std::binary_function<T,T,T> {
public:
    /// @memo calculates the binomial coefficient C(n,k)
    T operator()(T n, T k);
};

/**
 * @fn template<typename T> T binomial_factorization<T>::operator()(T n, T k)
 * @brief operator for calculating binomial coefficients
 *
 * @param[in] n parameter of binomial coefficient
 * @param[in] k parameter of binomial coefficient
 *
 * @return value of C(n,k)
 */
template<typename T> T binomial_factorization<T>::operator()(T n, T k) {
    k = std::max(n-k,k); // optimization to avoid long multiplications and overflow on division
    assert(n >= k);
    assert(n >= 0);
    assert(k >= 0);
    std::vector<T> numerator;
    std::vector<T> denominator;
    for(T i = k+1; i < n+1; ++i){
        numerator.push_back(i);
        denominator.push_back(n - i + 1);
    }
    factorization<T,int> result;
    // fold factorization over the list of numerator multipliers
    result = std::accumulate(numerator.begin(),numerator.end(),result, [](decltype(result)& r,T item){ return r*factorization<T,int>(item);});
    // fold factorization over the list of denominator multipliers
    result = std::accumulate(denominator.begin(), denominator.end(), result, [](decltype(result)& r,T item){ return r/factorization<T,int>(item);});
    return (T)result;
}
#endif //BINOMIAL_BUNOMIAL_FACTORIZATION_H
