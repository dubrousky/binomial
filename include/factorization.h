//
// Created by Aliaksandr Dubrouski on 6/8/15.
//

#ifndef BINOMIAL_PRIMEFACTORS_H
#define BINOMIAL_PRIMEFACTORS_H


#include <iosfwd>
#include <sstream>
#include <utils.h>

/*
 * @class Factorization
 * @brief Result of factorization of number into primes.
 *
 * This class represents the factorization of any number (int16) into the
 * product of the prime numbers: a = p1^n1*p2^n2*..pn^nn
 */
template<typename T, typename E>
class factorization {
public:
    // @brief default constructor for factorization of 1
    factorization() : factors() { }
    explicit factorization(T n);
    /// @brief multiply two numbers in factorized representation
    factorization<T,E> & operator*(const factorization<T,E>& rhs);
    /// @brief divide two numbers in factorized representation
    factorization<T,E> & operator/(const factorization<T,E>& rhs);
    /// @brief convert into number from the factorized implemetation
    operator T() const;
    operator std::string() const;

private:
    /// @brief basis of factorization the number is going to be decomposed
    std::map<T,E> factors;
};
/**
 * @fn template<typename T, typename E> Factorization<T,E>::Factorization(T n)
 * @brief Constructor is used to factorize the number submitted into primes
 *
 * Constructor fills in the prime powers corresponding to the decomposition
 * of supplied number into primes.
 *
 * @param n number to provide decomposition for.
 */
template<typename T, typename E> factorization<T,E>::factorization(T n) {
    //assert(n>=0 && n<=std::numeric_limits<unsigned short>::max());
    if( n>1 ){
        // create the map of primes the decomposition is performed over
        utils::prime_factors<T,E>(n,this->factors);
    }
}
/**
 * @fn template<typename T, typename E> Factorization<T,E> & Factorization<T,E>::operator*(const Factorization<T,E>& rhs)
 * @brief left associative multiplication operator, modifies the inital value
 *
 * @param[in] rhs Another number in the factorized form
 */
template<typename T, typename E> factorization<T,E> & factorization<T,E>::operator*(const factorization<T,E>& rhs) {
    //std::cout<<__PRETTY_FUNCTION__<<std::endl;
    // fold(rhs.factor,factors,(map,pair)->map)
    std::for_each(rhs.factors.cbegin(),
                    rhs.factors.cend(),
                    [this](std::pair<T,E> p) { this->factors[p.first] +=p.second; });
    return *this;
}

/**
 * @fn template<typename T, typename E> Factorization<T,E> & Factorization<T,E>::operator/(const Factorization<T,E>& rhs)
 * @brief left associative divison operator, modifies the inital value
 *
 * @param rhs Another number in the factorized form
 */
template<typename T, typename E>
factorization<T,E> & factorization<T,E>::operator/(const factorization<T,E>& rhs) {
    //std::cout<<__PRETTY_FUNCTION__<<std::endl;
    std::for_each(rhs.factors.cbegin(),
                  rhs.factors.cend(),
                    [this](std::pair<T,E> p) { this->factors[p.first] -=p.second; });
    return *this;
}

// The method converts the factorization back to number
template<typename T, typename E>
factorization<T,E>::operator T() const {
    return factors.empty() ? 1 : std::accumulate(factors.begin(),factors.end(),(T) factorization<T,E>(),[](T e,std::pair<T,E> p) {
        return e * pow(p.first,p.second);
    });
}

// The method converts the factorization into the string representation
template<typename T,typename E>
factorization<T,E>::operator std::string() const {
    std::stringstream ss;
    std::string result;
    ss << "1 *";
    std::for_each(factors.begin(),factors.end(),[&ss](std::pair<T,E> p) {
        if(p.second != 0)
            ss << p.first << "^" << p.second << " * ";
    });
    ss >> result;
    return result.substr(0,result.length()-1);
}


#endif //BINOMIAL_PRIMEFACTORS_H
