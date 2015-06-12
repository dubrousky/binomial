//
// Created by Aliaksandr Dubrouski on 6/10/15.
//

#include <numeric>
#include <assert.h>

#ifndef BINOMIAL_BINOMIAL_H
#define BINOMIAL_BINOMIAL_H

/**
 * @class binomial<T>
 * @brief Functor for binomial coefficient C(n,k) calculation.
 *
 * This class defines the dumb and inefficient way of test calculation
 * of C(n,k) = n!/(k!*(n-k)!) = P(k+1,..,n)/P(1,..,n-k); P stands for product.
 *
 */
template <typename T> class binomial : public std::binary_function<T,T,T> {
public:
    /// @brief calculates the binomial coefficient C(n,k)
    T operator()(T n, T k);
};

/**
 * @brief binomial coefficient calculation according to the formula C(n,k) = P(k+1,..,n)/P(1,..,n-k)
 *
 * @param[in] n parameter of binomial coefficient
 * @param[in] k parameter of binomial coefficient
 *
 * @return value of C(n,k)
 */
template<typename T> T binomial<T>::operator()(T n, T k) {
    assert(n >= k);
    assert(n > 0);
    assert(k >= 0);
    k = std::max(n-k,k); // optimization to avoid long multiplications and overflow on division
    std::vector<T> numerator;
    std::vector<T> denominator;
    std::multiplies<T> op;

    for(T i = k+1; i < n+1; ++i){
        numerator.push_back(i);
        denominator.push_back(n - i + 1);
    }
    T denom = std::accumulate(denominator.begin(),denominator.end(),1,op);
    T num = std::accumulate(numerator.begin(),numerator.end(),1,op);
    return num / denom;
}
#endif //BINOMIAL_BINOMIAL_H
