//
// Created by Aliaksandr Dubrouski on 6/10/15.
//
#ifndef BINOMIAL_BINOMIAL1_H
#define BINOMIAL_BINOMIAL1_H
#include <thread>
#include <vector>
#include <assert.h>
#include <future>
#include <numeric>
#include <paccumulate.h>
#include <binomial1.h>



template<unsigned int T>
class threading_policy {
public:
    enum { value = T };
};

/**
 * binary operation that is used in paccumulate must be a monoid to be splitted into
 * the independent concurrent operations that are joined after completion of each operation.
 */
template<typename T>
class multiplication_monoid : public LeftBinOpWithUnity<std::multiplies<T> >{
public:
    multiplication_monoid() : LeftBinOpWithUnity<std::multiplies<T> >(T(1)) {}
};

/**
 * binary operation that is used in paccumulate must be a monoid to be splitted into
 * the independent concurrent operations that are joined after completion of each operation.
 *
 * division is considered as multiplication of fractions:
 *
 * 1/(a b) = (1/a)*(1/b), with the unity element 1/1: (1/1)*(1/a) = (1/a) excluding 0
 */
template<typename T>
class division_monoid : public LeftBinOpWithUnity<std::divides<T> >{
public:
    division_monoid() : LeftBinOpWithUnity<std::divides<T> >(T(1)) {}
};
/**
 * @class binomial1<T>
 * @brief Functor for binomial coefficient C(n,k) calculation using std::async.
 *
 * This class defines the dumb and inefficient way of test calculation
 * of C(n,k) = n!/(k!*(n-k)!) = P(k+1,..,n)/P(1,..,n-k); P stands for product.
 *
 * The threads spawned on each part of calculation and efficiency depends on the
 * number of parts the calculation is divided and limited from above by the
 * Amdahl's law.
 *
 */
template <typename T> class binomial1 : public std::binary_function<T,T,T> {
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
template<typename T> T binomial1<T>::operator()(T n, T k) {
    k = std::max(n-k,k); // optimization to avoid long multiplications and overflow on division
    assert(n >= k);
    assert(n >= 0);
    assert(k >= 0);
    std::vector<T> numerator;
    std::vector<T> denominator;


    for(T i = k+1; i < n+1; ++i){
        numerator.push_back(i); // P(k+1,..,n)
        denominator.push_back(n - i + 1); // P(1,..,n-k);
    }
    // enhance the multiply operation with unity
    multiplication_monoid<T> op;

    T denom = paccumulate(denominator.begin(),denominator.end(),op.unity(),op,15);

    T num = paccumulate(numerator.begin(),numerator.end(),op.unity(),op,15);
    assert(0 != denom);
    return num / denom;
}


#endif //BINOMIAL_BINOMIAL1_H