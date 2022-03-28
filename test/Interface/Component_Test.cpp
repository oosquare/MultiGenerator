#include <iostream>
#include <filesystem>
#include <cassert>

#include <MultiGenerator/Interface/Component.hpp>

namespace Workflow = MultiGenerator::Workflow;
namespace Variable = MultiGenerator::Variable;
namespace Interface = MultiGenerator::Interface;

class AddGenerator : public Interface::GeneratingTask {
private:
    void generate(std::ostream &data, const Variable::DataConfig &config) override {
        int a = std::stoi(config.get("a").value());
        int b = std::stoi(config.get("b").value());
        data << a << " " << b << std::endl;
    }
};

class AddSolution : public Interface::SolutionTask {
private:
    void solve(std::istream &dataIn, std::ostream &dataOut,
        const Variable::DataConfig &config) override {
        int a, b;
        dataIn >> a >> b;
        dataOut << a + b << std::endl;
    }
};

class IntegratedAddGenerator : public Interface::IntegratedGeneratingTask {
private:
    void generate(std::ostream &dataIn, std::ostream &dataOut,
        const Variable::DataConfig &config) override {
        int a = std::stoi(config.get("a").value());
        int b = std::stoi(config.get("b").value());
        dataIn << a << " " << b << std::endl;
        dataOut << a + b << std::endl;
    }
};

void testGeneratingTask() {
    {
        AddGenerator task;
        task.setProblemName("add");
        task.setArgument(std::make_shared<Variable::SubtaskArgument>(
            1,
            1,
            Variable::DataConfig::create({
                {"a", "1"},
                {"b", "2"}
            })
        ));
        task.initEnvironment();
        task.call();
    }

    {
        std::string str;
        std::getline(std::ifstream("add1-1.in"), str);
        assert(str == "1 2");
    }

    {
        namespace filesystem = std::filesystem;
        filesystem::remove(filesystem::path("add1-1.in"));
    }
}

void testSolutionTask() {
    {
        std::ofstream("add1-1.in") << 1 << " " << 2 << std::endl;
    }

    {
        AddSolution task;
        task.setProblemName("add");
        task.setArgument(std::make_shared<Variable::SubtaskArgument>(1, 1,
            Variable::DataConfig::create({})));
        task.initEnvironment();
        task.call();
    }

    {
        std::string str;
        std::getline(std::ifstream("add1-1.out"), str);
        assert(str == "3");
    }

    {
        namespace filesystem = std::filesystem;
        filesystem::remove(filesystem::path("add1-1.in"));
        filesystem::remove(filesystem::path("add1-1.out"));
    }
}

void testIntegratedGeneratingTask() {
    {
        IntegratedAddGenerator task;
        task.setProblemName("add");
        task.setArgument(std::make_shared<Variable::SubtaskArgument>(
            1,
            1,
            Variable::DataConfig::create({
                {"a", "1"},
                {"b", "2"}
            })
        ));
        task.initEnvironment();
        task.call();
    }

    {
        std::string str;
        std::getline(std::ifstream("add1-1.in"), str);
        assert(str == "1 2");
        std::getline(std::ifstream("add1-1.out"), str);
        assert(str == "3");
    }

    {
        namespace filesystem = std::filesystem;
        filesystem::remove(filesystem::path("add1-1.in"));
        filesystem::remove(filesystem::path("add1-1.out"));
    }
}

int main() {
    testGeneratingTask();
    testSolutionTask();
    testIntegratedGeneratingTask();
    return 0;
}