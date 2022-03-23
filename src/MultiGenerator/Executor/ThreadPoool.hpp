/**
 * @file MultiGenerator/Executor/ThreadPoool.hpp
 * @author Justin Chen (ctj12461@163.com)
 * @brief A thread pool which execute all class derived from Runner.
 * @version 0.1
 * @date 2022-03-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <queue>
#include <memory>

#include <MultiGenerator/Workflow/Runner.hpp>

namespace MultiGenerator::Executor {
    using MultiGenerator::Workflow::Runner;

    /**
     * @brief A struct which stores the status and all pending runners
     * of a thread pool.
     *
     */
    struct ThreadPoolStatus {
        std::mutex poolLock;
        std::condition_variable condVar;

        std::atomic_bool isStopped;
        std::atomic_int runningWorkerCount;
        int maxWorkerCount;
        std::queue<std::shared_ptr<Runner>> runnerQueue;

        ThreadPoolStatus() :
            isStopped(true),
            runningWorkerCount(0),
            maxWorkerCount(0) {}

        void setMaxWorkerCount(int maxWorkerCount) {
            isStopped = (maxWorkerCount == 0);
            this->maxWorkerCount = maxWorkerCount;
        }
    };

    /**
     * @brief The handle of the worker thread created by the thread pool.
     *
     */
    class Worker {
    public:
        /**
         * @brief Initialize a worker thread.
         *
         * @param status the status of the thread pool
         */
        void start(ThreadPoolStatus &status) {
            handle = std::thread([&, this]() {
                ++status.runningWorkerCount;

                while (true) {
                    /** Get a runner from the queue or quit if it's empty. */
                    auto runner = getRunner(status);

                    if (runner)
                        runner->call();
                    else
                        break;
                }

                --status.runningWorkerCount;
            });
        }

        /**
         * @brief Wait for the worker's quitting. The worker will stop
         * automatically by itself.
         *
         */
        void stop() {
            if (handle.joinable())
                handle.join();
        }
    private:
        std::thread handle;

        std::shared_ptr<Runner> getRunner(ThreadPoolStatus &status) {
            std::shared_ptr<Runner> runner;
            std::unique_lock lock(status.poolLock);
            /** Sleep until receiving a new task or a stop singal. */
            if (status.runnerQueue.empty() && !status.isStopped) {
                status.condVar.wait(lock, [&]() {
                    return (!status.runnerQueue.empty() || status.isStopped);
                });
            }
            /** Check the queue first to ensure finishing remain tasks before quitting. */
            if (!status.runnerQueue.empty()) {
                runner = std::move(status.runnerQueue.front());
                status.runnerQueue.pop();
            }

            return runner;
        }
    };

    class MaxThreadCountInvalidException : public std::exception {
    public:
        const char *what() const noexcept override {
            return "MaxThreadCountInvalidException: The max count of threads must be positive.";
        }
    };

    class ThreadPoolAlreadyStartedException : public std::exception {
    public:
        const char *what() const noexcept override {
            return "ThreadPoolAlreadyStartedException: The pool had already started.";
        }
    };

    class ThreadPoolAlreadyStoppedException : public std::exception {
    public:
        const char *what() const noexcept override {
            return "ThreadPoolAlreadyStoppedException: The pool had already stopped.";
        }
    };

    class ThreadPoolIsNotRunningException : public std::exception {
    public:
        const char *what() const noexcept override {
            return "ThreadPoolIsNotRunningException: The pool can't handle a runner when stopped.";
        }
    };

    class RunnerHandleInvalidException : public std::exception {
    public:
        const char *what() const noexcept override {
            return "RunnerHandleInvalidException: The handle (std::shared_ptr<Runner>) is empty.";
        }
    };

    /**
     * @brief A class which manages the workers and posts runners.
     * 
     */
    class ThreadPool {
    public:
        /**
         * @brief Construct a new stopped thread pool object. Need to call start()
         * to handle tasks.
         *
         */
        ThreadPool() :
            status(std::make_unique<ThreadPoolStatus>()) {}

        /**
         * @brief Construct a new thread pool object with maxWorkerCount worker(s)
         * running on the background.
         *
         * @param maxWorkerCount haw many worker(s) to create
         */
        ThreadPool(int maxWorkerCount) :
            status(std::make_unique<ThreadPoolStatus>()) {
            start(maxWorkerCount);
        }

        ThreadPool(const ThreadPool &rhs) = delete;

        ThreadPool(ThreadPool &&rhs) = default;

        ThreadPool &operator=(const ThreadPool &rhs) = delete;

        ThreadPool &operator=(ThreadPool &&rhs) = default;

        ~ThreadPool() {
            if (!status->isStopped)
                stop();
        }

        /**
         * @brief Start the thread pool with maxWorkerCount worker(s) running
         * on the background. Throw exception when maxWorkerCount is invalid or
         * the pool has already started.
         *
         * @param maxWorkerCount haw many worker(s) to create
         */
        void start(int maxWorkerCount) {
            if (maxWorkerCount <= 0)
                throw MaxThreadCountInvalidException();

            if (!status->isStopped)
                throw ThreadPoolAlreadyStartedException();

            status->setMaxWorkerCount(maxWorkerCount);
            workers.resize(static_cast<std::vector<Runner>::size_type>(maxWorkerCount));

            for (auto &worker : workers)
                worker.start(*status);

            /** Return before all workers finish initializing. */
            while (status->runningWorkerCount != status->maxWorkerCount) {}
        }

        /**
         * @brief Stop the thread pool after finishing all tasks in the queue.
         * Throw exception when the pool has already stopped.
         *
         */
        void stop() {
            if (status->isStopped)
                throw ThreadPoolAlreadyStoppedException();

            status->setMaxWorkerCount(0);
            /** Wake up all sleeping workers to quit first. */
            status->condVar.notify_all();
            /** Ensure that all workers stop first. */
            for (auto &worker : workers)
                worker.stop();
        }

        /**
         * @brief Construct a runner and put it into the queue directly.
         * 
         * @tparam RunnerDerived the runner derived from Runner
         * @tparam Args the type of arguments passed to the constructor of RunnerDerived
         * @param args the arguments passed to the constructor of RunnerDerived
         * @return a handle of the runner which is able to get some essential inforamation
         */
        template <typename RunnerDerived, typename ...Args>
        std::shared_ptr<Runner> execute(Args &&...args) {
            /** RunnerDerived must implement Runner . */
            static_assert(std::is_base_of_v<Runner, RunnerDerived>,
                "RunnerDerived must be a derived class of Runner.");

            auto runner = std::make_shared<RunnerDerived>(std::forward<Args>(args)...);
            execute(runner);
            return runner;
        }

        /**
         * @brief Put runner into the queue. Throw if the thread pool is stoppped
         * or the handle is empty.
         *
         * @param runner the runner handle
         */
        void execute(std::shared_ptr<Runner> runner) {
            if (status->isStopped)
                throw ThreadPoolIsNotRunningException();

            if (!runner)
                throw RunnerHandleInvalidException();

            std::unique_lock<std::mutex> lock(status->poolLock);
            status->runnerQueue.push(std::move(runner));
            lock.unlock();
            /** Try to wake up a slleeping worker to handle this runner. */
            status->condVar.notify_one();
        }
    private:
        std::unique_ptr<ThreadPoolStatus> status;
        std::vector<Worker> workers;
    };
} // namespace MultiGenerator::Executor
