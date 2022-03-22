#include <iostream>
#include <memory>
#include <thread>
#include <cassert>

#include <MultiGenerator/Workflow/Runner.hpp>

namespace Workflow = MultiGenerator::Workflow;

void testRunnerCallOnce() {
    class TestRunner : public Workflow::Runner {
    public:
        TestRunner() :
            count(0) {};

        int getCount() const {
            return count;
        }
    private:
        std::atomic_int count;

        void run() override {
            ++count;
        }
    };
    
    TestRunner runner;
    std::thread t[8];

    for (int i = 0; i < 8; ++i) {
        t[i] = std::thread([&]() {
            runner.call();
        });
    }

    for (int i = 0; i < 8; ++i)
        t[i].join();

    assert(runner.getCount() == 1);
}

void testRunnerGetStatus() {
    class TestRunner : public Workflow::Runner {
    private:
        void run() override {
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    };

    TestRunner runner;

    assert(runner.getStatus() == Workflow::Runner::Status::Pending);

    std::thread t([&]() {
        runner.call();
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));
    assert(runner.getStatus() == Workflow::Runner::Status::Running);

    t.join();
    assert(runner.getStatus() == Workflow::Runner::Status::Finished);
}

int main() {
    testRunnerCallOnce();
    testRunnerGetStatus();
    return 0;
}