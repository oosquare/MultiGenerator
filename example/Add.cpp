/**
 * @file Add.cpp
 * @author Justin Chen (ctj12461@163.com)
 * @brief A demo of A + B Problem.
 * @version 0.1
 * @date 2022-04-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <random>
#include <MultiGenerator.hpp>

using MultiGenerator::DataConfig;
using MultiGenerator::GeneratingTask;
using MultiGenerator::SolutionTask;
using MultiGenerator::NormalTemplate;
using MultiGenerator::entry;
using MultiGenerator::testcase;

class AddGenerator : public GeneratingTask {
private:
    void generate(std::ostream &data, const DataConfig &config) override {
        auto minValue = std::stoi(config.get("minValue").value());
        auto maxValue = std::stoi(config.get("maxValue").value());

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(minValue, maxValue);

        data << dist(gen) << " " << dist(gen) << std::endl;
    }
};

class AddSolution : public SolutionTask {
private:
    void solve(std::istream &dataIn, std::ostream &dataOut, const DataConfig &) override {
        int a, b;
        dataIn >> a >> b;
        dataOut << a + b << std::endl;
    }
};

int main() {
    constexpr int MAX_THREAD_COUNT = 8;
    constexpr int MAX_TESTCASE_COUNT = 20;
    constexpr char PROBLEM_NAME[] = "add";

    MultiGenerator::NormalTemplate temp(PROBLEM_NAME);

    for (int i = 0; i < MAX_TESTCASE_COUNT; ++i) {
        temp.add<AddGenerator, AddSolution>(testcase(i, {
            entry("minValue", i * 1000000),
            entry("maxValue", (i + 1) * 1000000)
        }));
    }

    temp.execute(MAX_THREAD_COUNT);
    return 0;
}