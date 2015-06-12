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

template< class T > class thread_pool;

/**
 * @class worker
 * @brief class for picking the job from the thread pool task queue.
 *
 * This is a class for performing piece of work available from the
 * work queue of the thread pool. In the contrast to the std::async
 * it does not spawn the background thread - it uses the predefined
 * number of worker threads.
 */
template<class T>
class worker {
public:
    /// @brief constructor
    explicit worker(thread_pool<T>& s) : pool(s) { }
    /// @brief operator for executing the event loop
    void operator()();
private:
    /// @memo link to the thread pool to get the
    thread_pool<T>& pool;
};

/**
 * @fn template<class T> void worker<T>::operator()()
 * @brief Runs the execution cycle in the thread.
 *
 * The thread executes the following cycle:
 * - waits for task to be available on the queue
 * - executes the task
 */
template<class T> void worker<T>::operator()()
{
    T task;
    while(true)
    {
        {   // acquire lock
            std::unique_lock<std::mutex> lock(pool.queue_mutex);

            // look for a work item
            while(!pool.stopping && pool.tasks.empty())
            { // if there are none wait for notification
                pool.work_available.wait(lock);
            }

            if(pool.stopping) // exit if the pool is stopped
                return;

            // get the task from the queue
            task = std::move(pool.tasks.front());

            pool.tasks.pop();

        }   // release lock

        // execute the task to complete the associated future
        task();
    }
}
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
template<class T>
class thread_pool {
public:
    /// @brief Constructor
    explicit thread_pool(size_t);

    /// @info No copy or assignment
    thread_pool(const thread_pool&) = delete;
    thread_pool& operator=(const thread_pool&) = delete;
    /// @brief Method to submit the work into the queue
    void submit(T f);
    /// @brief Destructor
    ~thread_pool();
private:
    friend class worker<T>;

    // Threads
    std::vector< std::thread > workers;

    // Task queue
    std::queue<T> tasks;

    mutable std::mutex queue_mutex;
    std::condition_variable work_available;
    bool stopping;
};

/**
 * @fn template<class T> void thread_pool<T>::submit(T f)
 *
 * @brief adds new work item to the pool's queue to wait for execution
 *
 * @param[in] f delayed task (usually std::packaged_task)
 */
template<class T> void thread_pool<T>::submit(T f) {
    { // acquire lock
        std::unique_lock<std::mutex> lock(queue_mutex);

        // add the task to the queue
        tasks.push(std::move(f));
    } // release lock

    // wake up one thread
    work_available.notify_one();
}

/**
 * @fn template<class T> thread_pool<T>::thread_pool(size_t threads)
 * @brief The constructor launches the configured amount of workers
 */
template<class T>
thread_pool<T>::thread_pool(size_t threads)
        :   stopping(false) {
    for(size_t i = 0;i<threads;++i)
        workers.push_back(std::thread(worker<T>(*this)));
}

/**
 * @fn template<class T> thread_pool<T>::~thread_pool()
 * @brief cancels the ongoing work and joins all threads
 */
template<class T>
thread_pool<T>::~thread_pool() {
    // stopping all threads
    stopping = true;
    work_available.notify_all();

    // join
    for(size_t i = 0;i<workers.size();++i)
        workers[i].join();
}

/**
 * @fn template<typename T, typename Job> std::future<T> async(Job job, thread_pool<std::packaged_task<T()> >& pool)
 * @brief Function analogous to std::async
 *
 * This is global method to  perform a packaged task calculation in the
 * thread pool
 *
 * @param[in] job Task to be peformed
 * @pool[in] pool Reference to the thread pool
 */
template<typename T, typename Job> std::future<T> async(Job job, thread_pool<std::packaged_task<T()> >& pool) {
    std::packaged_task<T()> task(std::move(job));
    auto f = task.get_future(); // get future to obtain result
    pool.submit(std::move(task));
    return f;
}
#endif //BINOMIAL_THREADPOOL_H
