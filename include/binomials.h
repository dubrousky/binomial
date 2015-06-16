//
// Created by Aliaksandr Dubrouski on 6/16/15.
//

#ifndef BINOMIAL_BINOMIALS_H
#define BINOMIAL_BINOMIALS_H
#include <binomial.h>
#include <binomial1.h>
#include <binomial_factorization.h>
#include <binomial_factorization1.h>

// This is a front end for all created functions for binomial calculation.

// Parametrization of algorithm with the calculation strategies
namespace calculate {
    /// @info calculation of binomial coefficients based on sequential multiplication
    class seq_multiply {
    };
    /// @info calculation of binomial coefficients based on sequential factorization
    class seq_factorize {
    };

    /// @info calculation of binomial coefficients based on parallel multiplication
    class async_multiply {
    };
    /// @info calculation of binomial coefficients based on parallel factorization
    class async_factorize {
    };
};


// TODO: add parametrization of concurrent algorithms with execution context (thread pool or std::async)
// TODO: in production use boost::thread_group or Intel thread building block tbb::parallel_for/accumulate.
template<typename T, typename P>
class binomials { };

template<typename T> class binomials<T,calculate::seq_multiply> : public binomial<T> { };

template<typename T> class binomials<T,calculate::async_multiply> : public binomial1<T> { };

template<typename T> class binomials<T,calculate::seq_factorize> : public binomial_factorization<T> { };

template<typename T> class binomials<T,calculate::async_factorize> : public binomial_factorization1<T> { };

#endif //BINOMIAL_BINOMIALS_H
