/**
 * @file MultiGenerator/Workflow/Runner.hpp
 * @author Justin Chen (ctj12461@163.com)
 * @brief This file declares the abstract class of a runnable object.
 * @version 0.1
 * @date 2022-03-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <atomic>

namespace MultiGenerator::Workflow {
    /**
     * @brief An abstract class which can only execute once in multithreading
     * environmet. Put anything according to you in derived class such as
     * the returned value or arguments.
     *
     */
    class Runner {
    public:
        /**
         * @brief A status of one Runner.
         * 
         */
        enum class Status {
            Pending, Running, Finished
        };

        Runner() :
            status(Status::Pending) {}

        virtual ~Runner() {}

        /**
         * @brief The runner will only execute onec even though using call() more
         * than once.
         *
         */
        void call() {
            Status oldStatus = Status::Pending;

            if (!status.compare_exchange_strong(oldStatus, Status::Running))
                return;

            run();
            status = Status::Finished;
        }

        Status getStatus() const {
            return status;
        }
    protected:
        /**
         * @brief The runner will execute run(). Implement this function in
         * the derived class in order to use your features.
         *
         */
        virtual void run() = 0;
    private:
        std::atomic<Status> status;
    };
} // namespace MultiGenerator::Workflow

