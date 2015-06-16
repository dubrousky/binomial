//
// Created by Aliaksandr Dubrouski on 6/13/15.
//

#include <future>
#include "thread_pool.h"

/**
 * @fn void worker::operator()()
 * @brief Runs the execution cycle in the thread.
 *
 * The thread executes the following cycle:
 * - waits for task to be available on the queue
 * - executes the task
 */
void worker::operator()()
{
    std::future<void> task;
    while(true)
    {
        {   // acquire lock
            std::unique_lock<std::mutex> lock(pool.queue_mutex);

            // look for a work item
            while(!pool.stopping.load() && pool.tasks.empty())
            { // if there are none wait for notification
                pool.work_available.wait(lock);
            }
            if(pool.stopping.load()) // exit if the pool is stopped
                return;

            // get the task from the queue
            task = std::move(pool.tasks.front());
            pool.tasks.pop();

        }   // release lock

        // wait for result to be available
        task.wait();
    }
}

/**
 * @fn thread_pool::thread_pool(size_t threads)
 * @brief The constructor launches the configured amount of workers
 */

thread_pool::thread_pool(size_t threads)
        :   stopping(false) {
    for(auto i = 0;i<threads;++i)
        workers.push_back(std::thread(worker(*this)));
}

/**
 * @fn thread_pool::~thread_pool()
 * @brief cancels the ongoing work and joins all threads
 */
thread_pool::~thread_pool() {
    // stopping all threads
    while(!stopping.load()) stopping.store(true);
    work_available.notify_all();

    // join
    for(size_t i = 0;i<workers.size();++i)
        workers[i].join();
}
