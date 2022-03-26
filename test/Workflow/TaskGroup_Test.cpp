#include <iostream>
#include <string>
#include <memory>
#include <cassert>

#include <MultiGenerator/Variable/Argument.hpp>
#include <MultiGenerator/Workflow/TaskGroup.hpp>

namespace Variable = MultiGenerator::Variable;
namespace Workflow = MultiGenerator::Workflow;

class TestTask : public Workflow::Task {
public:
    TestTask() :
        Workflow::Task(),
        result() {}
    
    void call() override {
        result = std::string("TestTask: ") + arg->getConfig().get("result").value();
    }

    std::string getResult() const {
        return result;
    }
private:
    std::string result;
};

void testTaskGroup() {
    Workflow::TaskGroup group(std::make_shared<Variable::NormalArgument>(
        1,
        Variable::DataConfig::create({
            {"result", "successful test"}
        })
    ));

    int id = group.add([]() {
        return std::make_unique<TestTask>();
    });

    auto entry = group.next();
    assert(entry.value().id == id);

    auto task = entry.value().constructor();
    task->call();
    auto p = dynamic_cast<TestTask *>(task.get());
    assert(p->getResult() == "TestTask: successful test");
}

int main() {
    testTaskGroup();
    return 0;
}