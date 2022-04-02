#include <iostream>
#include <set>
#include <cassert>

#include <MultiGenerator/Variable/Argument.hpp>
#include <MultiGenerator/Workflow/TaskGroup.hpp>
#include <MultiGenerator/Executor/TaskExecutor.hpp>

namespace Variable = MultiGenerator::Variable;
namespace Workflow = MultiGenerator::Workflow;
namespace Executor = MultiGenerator::Executor;

class TestTask : public Workflow::Task {
public:
    TestTask(Executor::Sender<int> sender) :
        Workflow::Task(),
        sender(std::move(sender)) {}

    void call() override {
        auto id = arg->getConfig().get("num").value();
        bool res = sender.send(std::stoi(id));
        assert(res == true);
    }
private:
    Executor::Sender<int> sender;
};

class TestTask2 : public Workflow::Task {
public:
    void call() override {
        std::cout << "TestTask2\n";
    }
};

void testTaskExecutor() {
    constexpr int GROUP_COUNT = 1;

    Executor::TaskExecutor executor;
    std::vector<Workflow::TaskGroup> groups;
    Executor::Receiver<int> receiver;

    for (int i = 0; i < GROUP_COUNT; ++i) {
        Workflow::TaskGroup group(std::make_shared<Variable::NormalArgument>(
            i,
            Variable::DataConfig::create({
                {"num", std::to_string(i)}
            })
        ));

        group.add([&receiver]() {
            return std::make_unique<TestTask>(Executor::Channel<int>::open(receiver));
        });

        group.add([]() {
            return std::make_unique<TestTask2>();
        });

        groups.push_back(std::move(group));
    }

    executor.execute(groups, 8);

    std::multiset<int> result;
    
    while (true) {
        auto res = receiver.receive();

        if (!res.has_value())
            break;

        result.insert(res.value());
    }

    for (int i = 0; i < GROUP_COUNT; ++i)
        assert(result.count(i) == 1);
}

int main() {
    testTaskExecutor();
    return 0;
}