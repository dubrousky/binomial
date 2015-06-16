//
// Created by Aliaksandr Dubrouski on 6/15/15.
//

#ifndef BINOMIAL_BINOMIAL_FACTORIZATION1_H
#define BINOMIAL_BINOMIAL_FACTORIZATION1_H
#include<paccumulate.h>
// functional wrappers for factorized numbers multiplication/division
template<typename T, typename E>
class m : public std::binary_function<factorization<T, E>&,T, factorization<T, E>&> {
public:
    auto operator()(factorization<T,E>& b,T a ) -> factorization<T, E>& {
        return(b * factorization<T,E>(a),b);
    }
};

template<typename T, typename E>
class d : public std::binary_function<factorization<T,E>&,T, factorization<T,E>&> {
public:
    auto operator()(factorization<T,E>& b,T a) -> factorization<T,E>& {
        return(b / factorization<T,E>(a),b);
    }
};

/**
 * @class template <typename T> class binomial_factorization1
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
template <typename T> class binomial_factorization1 : public std::binary_function<T,T,T> {
public:
    /// @memo calculates the binomial coefficient C(n,k)
    T operator()(T n, T k);
};

/**
 * @fn template<typename T> T binomial_factorization1<T>::operator()(T n, T k)
 * @brief operator for calculating binomial coefficients in parallel
 *
 * @param[in] n parameter of binomial coefficient
 * @param[in] k parameter of binomial coefficient
 *
 * @return value of C(n,k)
 */
template<typename T> T binomial_factorization1<T>::operator()(T n, T k) {
    thread_pool_of<10> tp;
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

    LeftBinOpWithUnity<m<T,int> > mult(result);
    LeftBinOpWithUnity<d<T,int> > div(result);
    // fold factorization over the list of numerator multipliers
    result = paccumulate(numerator.begin(),numerator.end(),result, mult, 30);
    // fold factorization over the list of denominator multipliers and return
    return (T)paccumulate(denominator.begin(), denominator.end(), result, div, 30);
}
#endif //BINOMIAL_BINOMIAL_FACTORIZATION1_H
