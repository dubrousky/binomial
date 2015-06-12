#include<iostream>
#include <thread>
#include <vector>
#include <assert.h>
#include <future>
#include <math.h>
#include <thread_pool.h>
#include <map>
#include <binomial1.h>
#include <binomial.h>
#include <utils.h>
#include "factorization.h"
#include <binomial_factorization.h>


template<typename T> T binomial2(T n, T k);

int main(int argc, char** argv) {
    auto start(std::chrono::system_clock::now());
    std::string offset = "                              ";
    binomial<unsigned long long> b_ull;
    binomial1<unsigned long long> b1_ull;
    binomial_factorization<unsigned int> bf_ull;
    for( auto i = 0; i < 30; ++i ) {
        std::cout << offset;
        offset = offset.substr(1);
        for( auto j = 0; j <= i; ++j ) {
            std::cout.width(2);
            std::cout<< bf_ull(i,j)<<" ";
        }
        std::cout<<std::endl;
    }
    auto end(std::chrono::system_clock::now());
    std::cout<<"timing: "<<(end-start).count()<<std::endl;

    return 0;
}



