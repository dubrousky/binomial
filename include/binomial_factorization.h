//
// Created by Aliaksandr Dubrouski on 6/11/15.
//
#include<paccumulate.h>

#ifndef BINOMIAL_BUNOMIAL_FACTORIZATION_H
#define BINOMIAL_BUNOMIAL_FACTORIZATION_H

// decomposition of product of numbers into the product of prime powers. the implementation shall use the
// the sieve Eratosthenes of to factorize the numbers into primes:
// find all primes in the range (1..sqrt(n)), decompose each multiplier in the numerator and denominator in
// C(n,k) = P(k+1,...,n)/P(1,..,n-k), where P stands for product.
// The calculation is performed as follows: an array of primes from 1 to sqrt(n) is created. The decomposition
// into primes is calculated concurrently: the thread pool is created and each worker thread takes a number
// and calculates its decomposition into primes; the result of calculation is submitted into the result array of prime
// factor powers.
template <typename T> class binomial_factorization : public std::binary_function<T,T,T> {
public:
    /// @memo calculates the binomial coefficient C(n,k)
    T operator()(T n, T k);
};

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

    class m : public std::binary_function<factorization<T, int>&,T, factorization<T, int>&> {
    public:
        auto operator()(factorization<T,int>& b,T a ) -> factorization<T,int> {
            return(b * factorization<T,int>(a),b);
        }
    };

    class d : public std::binary_function<factorization<T,int>&,T, factorization<T,int>&> {
    public:
        auto operator()(factorization<T,int>& b,T a) -> factorization<T,int> {
            return(b / factorization<T,int>(a),b);
        }
    };
    factorization<T,int> result;
    LeftBinOpWithUnity<m> mult(result);
    LeftBinOpWithUnity<d> div(result);
    // fold factorization over the list of numerator multipliers
    result = paccumulate(numerator.begin(),numerator.end(),result, mult, 10);
    // fold factorization over the list of denominator multipliers
    return (T)paccumulate(denominator.begin(), denominator.end(), result, div, 10);
}
#endif //BINOMIAL_BUNOMIAL_FACTORIZATION_H
