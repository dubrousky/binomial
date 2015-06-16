#include<iostream>
#include <thread>
#include <vector>
#include <assert.h>
#include <future>
#include <thread_pool.h>
#include <map>
#include <binomial1.h>
#include <binomial.h>
#include "factorization.h"
#include <binomial_factorization.h>
#include <binomial_factorization1.h>
#include <utils.h>
#include <binomials.h>


/// @brief the function calculates the C(n,k) of Pascals triangle for illustration
int main(int argc, char** argv) {
    binomials<unsigned int,calculate::seq_multiply>    bs0;
    binomials<unsigned int,calculate::async_multiply>  bs1;
    binomials<unsigned int,calculate::seq_factorize>   bs2;
    binomials<unsigned int,calculate::async_factorize> bs3;

    // faster than non-concurrent version
    // TODO: try to use thread pool instead of std::async
    utils::timing([&](){
        for( auto i = (unsigned int)0; i < 100; ++i ) {
            for( auto j = (unsigned int)0; j <= i; ++j ) {
                std::cout.width(2);
                std::cout<< bs3(i,j)<<" ";
            }
            std::cout<<std::endl;
        }
    });
    // Slower than direct multiplication but covers entire range of unsigned short.
    utils::timing([&](){
        for( auto i = (unsigned int)0; i < 100; ++i ) {
            for( auto j = (unsigned int)0; j <= i; ++j ) {
                std::cout.width(2);
                std::cout<< bs2(i,j)<<" ";
            }
            std::cout<<std::endl;
        }
    });
    // Not so fast and limited to n=19 algorithm. Needs number of threads and fork threshold
    // tuning.
    // TODO: try to use thread pool instead of std::async
    utils::timing([&](){
        for( auto i = (unsigned int)0; i < 20; ++i ) {
            for( auto j = (unsigned int)0; j <= i; ++j ) {
                std::cout.width(2);
                std::cout<< bs1(i,j)<<" ";
            }
            std::cout<<std::endl;
        }
    });
    // The fastest but limited to n=19 algorithm
    utils::timing([&](){
        for( auto i = (unsigned int)0; i < 20; ++i ) {
            for( auto j = (unsigned int)0; j <= i; ++j ) {
                std::cout.width(2);
                std::cout<< bs0(i,j)<<" ";
            }
            std::cout<<std::endl;
        }
    });



    std::cout<<std::endl;


    return 0;
}



