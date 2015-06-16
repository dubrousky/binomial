//
// Created by Aliaksandr Dubrouski on 6/8/15.
//

#ifndef BINOMIAL_THREADPOOL_H
#define BINOMIAL_THREADPOOL_H
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <future>
#include <queue>
#include <iostream>
#include <numeric>
#include <memory>

class thread_pool;
/**
 * @class worker
 * @brief class for picking the job from the thread pool task queue.
 *
 * This is a class for performing piece of work available from the
 * work queue of the thread pool. In the contrast to the std::async
 * it does not spawn the background thread - it uses the predefined
 * number of worker threads.
 */
class worker {
public:
    /// @brief constructor
    explicit worker(thread_pool& s) : pool(s) { }
    /// @brief operator for executing the event loop
    void operator()();
private:
    /// @memo link to the thread pool to get the
    thread_pool& pool;
};

/**
 * @class thread_pool
 *
 * @brief Class for execution of delayed tasks on the group of threads
 *
 *
 * This is a thread pool class that manages the task queue
 * and threads required to perform the work. We will use the
 * std::packaged_task to submit into the thread pool and the
 * std::future linked to the packaged task
 */

class thread_pool {
public:
    /// @brief Constructor
    explicit thread_pool(size_t);

    /// @info No copy or assignment
    thread_pool(const thread_pool&) = delete;
    thread_pool& operator=(const thread_pool&) = delete;

    /// @brief Destructor
    ~thread_pool();
    /// @brief Submits task of typr void->void to the thread pool for execution
    template <template<typename> class Task>
    std::future<void> async(Task<void()> f);
    /// @brief Submits task of typr (args...)->void to the thread pool for execution
    template<typename... Args, template<typename> class Task>
    std::future<void> async(Task<void(Args...)> f, Args... args);

    /// @brief Submits task of typr (void)->ret to the thread pool for execution
    template<typename Ret, template<typename> class Task>
    std::future<Ret> async(Task<Ret()> f);
    /// @brief Submits task of typr (args...)->ret to the thread pool for execution
    template<typename Ret, typename... Args, template<typename> class Task>
    std::future<Ret> async(Task<Ret(Args...)> f, Args... args);

    /// @brief Executes the parallel version of std::accumulate on the thread pool
    template<typename R, typename Iterator, typename BinOp >
    R paccumulate(Iterator first,
                  Iterator last,
                               R start,
                               BinOp bop,
                               int fork_threshold = std::thread::hardware_concurrency());

private:
    friend class worker;

    // Threads
    std::vector< std::thread > workers;

    // Task queue
    std::queue<std::future<void> > tasks;

    mutable std::mutex queue_mutex;
    std::condition_variable work_available;
    std::atomic<bool> stopping;
};



/// @brief
template<typename Ret, typename... Args, template<typename> class Task>
std::future<Ret> thread_pool::async(Task<Ret(Args...)> f, Args... args) {
    typedef Task<Ret(Args...)> F;
    auto p = std::make_shared<std::promise<Ret> >(); // promise to be completed
    // packaged task to evaluate function and complete a promise
    auto task_wrapper = [p](F&& t,Args&&... as) {
        try {
            auto tmp = t(as...);
            p->set_value(std::move(tmp));
        } catch (...) {
            p->set_exception(std::current_exception());
        }
    };
    auto task = std::async(std::launch::deferred,task_wrapper,f,args...);
    {
        std::lock_guard<std::mutex> guard(queue_mutex);
        tasks.push(std::move(task));
    }
    work_available.notify_one();
    return p->get_future();
}
/// @brief unary function void -> Ret
template<typename Ret, template<typename> class Task>
std::future<Ret> thread_pool::async(Task<Ret()> f) {
    typedef Task<Ret()> F;
    auto p = std::make_shared<std::promise<Ret> >(); // promise to be completed
    // packaged task to evaluate function and complete a promise
    auto task_wrapper = [p](F&& t) {
        try {
            p->set_value(t());
        } catch (...) {
            p->set_exception(std::current_exception());
        }
    };

    auto task = std::async(std::launch::deferred,task_wrapper,std::move(f));
    {
        std::lock_guard<std::mutex> guard(queue_mutex);
        tasks.push(std::move(task));
    }
    work_available.notify_one();
    return p->get_future();
}
/// @brief
template<typename... Args,template<typename> class Task>
std::future<void> thread_pool::async(Task<void(Args...)> f, Args... args) {
    typedef Task<void(Args...)> F;
    auto p = std::make_shared<std::promise<void> >(); // promise to be completed
    // packaged task to evaluate function and complete a promise
    auto task_wrapper = [p](F&& t,Args&&...as) {
        try {
            t(as...);
            p->set_value();
        } catch (...) {
            p->set_exception(std::current_exception());
        }
    };

    auto task = std::async(std::launch::deferred,task_wrapper,f,args...);
    {
        std::lock_guard<std::mutex> guard(queue_mutex);
        tasks.push(std::move(task));
    }
    work_available.notify_one();
    return p->get_future();
}

/// @brief void -> void
template <template<typename> class Task>
std::future<void> thread_pool::async(Task<void()> f) {
    typedef Task<void()> F;
    auto p = std::make_shared<std::promise<void> >(); // promise to be completed
    // packaged task to evaluate function and complete a promise
    auto task_wrapper = [p](F&& t) {
        try {
            t();
            p->set_value();
        } catch (...) {
            p->set_exception(std::current_exception());
        }
    };

    auto task = std::async(std::launch::deferred,task_wrapper,std::move(f));
    {
        std::lock_guard<std::mutex> guard(queue_mutex);
        tasks.push(std::move(task));
    }
    work_available.notify_one();
    return p->get_future();
}

/// @brief (coll,start,op)->decltype(start)
template<typename R, typename Iterator, typename BinOp>
R thread_pool::paccumulate(Iterator first,Iterator last, R start, BinOp bop, int fork_threshold) {
    std::vector<std::future<R> > intermediates;
    // fork if the task volume is large enough
    while(std::distance(first,last) > fork_threshold) {
        Iterator mid = first;
        std::advance(mid,fork_threshold);
        auto handle = this->async([first,mid,bop]() -> R { return std::accumulate(first,mid,bop.unity(),bop);});
        intermediates.push_back(std::move(handle));
        std::advance(first,fork_threshold);
    }
    // calculate the rest itself if any
    R res = (first!=last) ? res = std::accumulate(first, last, start, bop) : start;
    // join the results (the load may be uneven)
    return std::accumulate(intermediates.begin(),
                           intermediates.end(),
                           res,[&bop](R e,std::future<R>& f) {
                return bop(e,f.get());
            });
}


/**
 * @class thread_pool_of<N>
 * @brief convenience wrapper for creation of threads with compile time deduced number of threads
 */
template<unsigned short N>
class thread_pool_of : public thread_pool {
public:
    thread_pool_of() : thread_pool(N){ }
};
#endif //BINOMIAL_THREADPOOL_H
