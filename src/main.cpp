#include<iostream>
#include <bits/stl_numeric.h>
#include <thread>
#include <vector>
#include <assert.h>
#include <future>

template<typename T> T binomial(T, T);
template<typename T> T binomial1(T, T);
template<typename Iterator, typename BinaryOp, typename T>  T paccumulate(Iterator first,Iterator last, T start, BinaryOp op);

int main(int argc, char** argv) {
    std::chrono::time_point start(std::chrono::system_clock::now());
    std::string offset = "                              ";
    for( unsigned long long i = 0; i < 20; ++i ) {
        std::cout << offset;
        offset = offset.substr(1);
        for( unsigned long long j = 0; j < i; ++j ) {
            std::cout.width(2);
            std::cout<< binomial1(i,j)<<" ";
        }
        std::cout<<std::endl;
    }
    std::chrono::time_point end(std::chrono::system_clock::now());
    std::cout<<end-start<<std::endl;
    return 0;
}
// TODO: add shortcirkit for some special cases: C(n,0), C(n,1), C(n,n-1), C(n,n), C(n,k) = C(n-1,k-1) + C(n-1,k), C(n,k) == C(n,n-k)
template<typename T> T binomial(T n, T k) {
    k = std::max(n-k,k); // optimisation to avoid long multiplications and overflow on division
    assert(n >= k);
    assert(n > 0);
    assert(k >= 0);
    std::vector<T> numerator;
    std::vector<T> denominator;
    std::multiplies<T> op;

    for(T i = k+1; i < n+1; ++i){
        numerator.push_back(i);
        denominator.push_back(n - i + 1);
    }
    T denom = accumulate(denominator.begin(),denominator.end(),1,op);
    T num = accumulate(numerator.begin(),numerator.end(),1,op);
    return num / denom;
}

template<typename T> T binomial1(T n, T k) {
     k = std::max(n-k,k); // optimization to avoid long multiplications and overflow on division
    assert(n >= k);
    assert(n > 0);
    assert(k >= 0);
    std::vector<T> numerator;
    std::vector<T> denominator;
    std::multiplies<T> op;
    for(T i = k+1; i < n+1; ++i){
        numerator.push_back(i);
        denominator.push_back(n - i + 1);
    }
    T denom = paccumulate(denominator.begin(),denominator.end(),1,op);
    T num = paccumulate(numerator.begin(),numerator.end(),1,op);
    assert(0 != denom);
    return num / denom;
}

// naive fork-join for any number of threads
template<typename Iterator, typename BinaryOp, typename T>  T paccumulate(Iterator first, Iterator last, T start, BinaryOp op) {
    Iterator mid = first;
    int threshold = 5;
    T unity  = (T)1;
    if(std::distance(first,last) < threshold/*threshold*/) { // calculate by itself if problem is slow or no more threads
        return std::accumulate(first, last, start, op);
    }
    std::advance(mid,threshold);
    auto handle = std::async(std::launch::async,
                             [](Iterator f, Iterator l, T s, BinaryOp o){ return std::accumulate(f,l,s,o); },
                             first, mid, start, op);
    return op(paccumulate(mid, last, unity, op), handle.get());
}