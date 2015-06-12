//
// Created by Aliaksandr Dubrouski on 6/11/15.
//

#ifndef BINOMIAL_PACCUMULATE_H
#define BINOMIAL_PACCUMULATE_H
#include<thread_pool.h>

// Mix in unity information to the binary operation that satisfies the fold-left criteria
template<typename Operation>
class LeftBinOpWithUnity : public Operation {
public:
    /// @memo constructor
    LeftBinOpWithUnity(typename Operation::first_argument_type unity) : u(unity) {};

    /// @memo returns left unity with property op(left_unity(),T) == T for any T by value
    typename std::remove_reference<typename Operation::first_argument_type>::type unity() const {return u;}

private:
    typename std::remove_reference<typename Operation::first_argument_type>::type u;
};

// thread pool based fork-join accumulate for any number of threads available
template<typename Iterator, typename T, typename BinOp>
T paccumulate(Iterator first, Iterator last, T start, BinOp bop, int fork_threshold = std::thread::hardware_concurrency()) {

    // futures of intermediate results
    std::vector<std::future<T> > intermediates;
    // fork if the task volume is large enough
    while(std::distance(first,last) > fork_threshold) {
        auto mid = first;
        std::advance(mid,fork_threshold);
        auto handle = std::async(std::launch::async,
                                 [first,mid,bop]() { return std::accumulate(first,mid,bop.unity(),bop);});
        intermediates.push_back(std::move(handle));
        std::advance(first,fork_threshold);
    }
    // calculate the rest itself
    T res = (first!=last) ? std::accumulate(first, last, start, bop) : start;

    // join the results from futures
    return std::accumulate(intermediates.begin(),
                           intermediates.end(),
                           res,[&bop](T e,std::future<T>& f) {
                return bop(e,f.get());
            });
}

template<typename Iterator, typename T, typename BinOp>
T paccumulate_pooled(Iterator first, Iterator last, T start, BinOp bop, int fork_threshold = std::thread::hardware_concurrency()) {
    std::vector<std::future<T> > intermediates;
    thread_pool<std::packaged_task<T()>> tp(2);
    // fork if the task volume is large enough
    while(std::distance(first,last) > fork_threshold) {
        Iterator mid = first;
        std::advance(mid,fork_threshold);
        auto handle = async([first,mid,bop]() -> T { return std::accumulate(first,mid,bop.unity(),bop);},tp);
        intermediates.push_back(std::move(handle));
        std::advance(first,fork_threshold);
    }
    // calculate the rest itself if any
    T res = (first!=last) ? res = std::accumulate(first, last, start, bop) : start;
    // join the results
    return std::accumulate(intermediates.begin(),
                           intermediates.end(),
                           res,[&bop](T e,std::future<T>& f) {
                return bop(e,f.get());
            });
}
#endif //BINOMIAL_PACCUMULATE_H
