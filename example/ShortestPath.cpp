/**
 * @file ShortestPath.cpp
 * @author Justin Chen (ctj12461@163.com)
 * @brief A demo of calculating the shortest path from the first node to
 * the last node of an undirected graph.
 * @version 0.1
 * @date 2022-04-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <random>
#include <vector>
#include <tuple>
#include <queue>
#include <cmath>
#include <MultiGenerator.hpp>

using MultiGenerator::DataConfig;
using MultiGenerator::GeneratingTask;
using MultiGenerator::SolutionTask;
using MultiGenerator::NormalTemplate;
using MultiGenerator::entry;
using MultiGenerator::testcase;

class RandomGraphGenerator : public GeneratingTask {
private:
    void generate(std::ostream &data, const DataConfig &config) override {
        int vertixCount = std::stoi(config.get("vertixCount").value());
        int maxEdgeCount = std::stoi(config.get("maxEdgeCount").value());
        int maxWeight = std::stoi(config.get("maxWeight").value());

        std::random_device rd;
        std::mt19937 gen(rd());

        data << vertixCount << " " << maxEdgeCount << std::endl;
        /** Generate a tree first. */
        for (int i = 2; i <= vertixCount; ++i) {
            data << getInt(gen, 1, i - 1) << " ";
            data <<  i << " ";
            data << getInt(gen, 1, maxWeight) << std::endl;
        }
        
        /** Generate the last maxEdgeCount - (vertixCount - 1) edges. */
        for (int i = 1; i <= maxEdgeCount - (vertixCount + 1); ++i) {
            data << getInt(gen, 1, vertixCount) << " ";
            data << getInt(gen, 1, vertixCount) << " ";
            data << getInt(gen, 1, vertixCount) << std::endl;
        }
    }

    int getInt(std::mt19937 &gen, int l, int r) {
        std::uniform_int_distribution<> dist(l, r);
        return dist(gen);
    }
};

class GridGraphGenerator : public GeneratingTask {
public:
    GridGraphGenerator() :
        row(0),
        column(0) {}
private:
    int row;
    int column;

    void generate(std::ostream &data, const DataConfig &config) override {
        int vertixCount = std::stoi(config.get("vertixCount").value());
        int maxWeight = std::stoi(config.get("maxWeight").value());

        row = static_cast<int>(std::floor(std::sqrt(vertixCount)));
        column = static_cast<int>(std::ceil(1.0 * vertixCount / row));

        std::random_device rd;
        std::mt19937 gen(rd());
        std::vector<std::tuple<int, int, int>> edges;

        for (int i = 1; i <= row; ++i) {
            for (int j = 1; j < column; ++j) {
                if (id(i, j) > vertixCount || id(i, j + 1) > vertixCount)
                    continue;
                
                edges.emplace_back(id(i, j), id(i, j + 1), getInt(gen, 1, maxWeight));
            }
        }

        for (int i = 1; i < row; ++i) {
            for (int j = 1; j <= column; ++j) {
                if (id(i, j) > vertixCount || id(i + 1, j) > vertixCount)
                    continue;
                
                edges.emplace_back(id(i, j), id(i + 1, j), getInt(gen, 1, maxWeight));
            }
        }

        data << vertixCount << " " << edges.size() << std::endl;

        for (auto [x, y, w] : edges)
            data << x << " " << y << " " << w << std::endl;
    }

    int id(int x, int y) {
        return (x - 1) * column + y;
    }

    int getInt(std::mt19937 &gen, int l, int r) {
        std::uniform_int_distribution<> dist(l, r);
        return dist(gen);
    }
};

class ShortestPathSolution : public SolutionTask {
private:
    using Edge = std::pair<int, int>;
    static constexpr int INF = 0x3f3f3f3f;

    void solve(std::istream &dataIn, std::ostream &dataOut, const DataConfig &) override {
        int vertixCount, edgeCount;
        dataIn >> vertixCount >> edgeCount;

        std::vector<std::vector<Edge>> graph(vertixCount + 1);

        for (int i = 1; i <= edgeCount; ++i) {
            int x, y, w;
            dataIn >> x >> y >> w;
            graph[x].emplace_back(y, w);
            graph[y].emplace_back(x, w);
        }
        
        dataOut << shortestPath(graph) << std::endl;
    }

    int shortestPath(const std::vector<std::vector<Edge>> &graph) {
        int vertixCount = graph.size() - 1;
        std::vector<bool> vis(vertixCount + 1);
        std::vector<int> dis(vertixCount + 1, INF);
        std::priority_queue<Edge, std::vector<Edge>, std::greater<Edge>> q;

        dis[1] = 0;
        q.push({ dis[1], 1 });

        while (!q.empty()) {
            int x = q.top().second;
            q.pop();

            if (vis[x])
                continue;

            vis[x] = true;

            for (auto [y, w] : graph[x]) {
                if (dis[y] <= dis[x] + w)
                    continue;

                dis[y] = dis[x] + w;
                q.push({ dis[y], y });
            }
        }

        return dis[vertixCount];
    }
};

int main() {
    NormalTemplate temp("graph");
    /** Subtask 1 */
    for (int i = 1; i <= 10; ++i) {
        temp.add<RandomGraphGenerator, ShortestPathSolution>(testcase(1, i, {
            entry("vertixCount", i * 10000),
            entry("maxEdgeCount", i * 50000),
            entry("maxWeight", 10000)
        }));
    }
    /** Subtask 2 */
    for (int i = 1; i <= 10; ++i) {
        temp.add<GridGraphGenerator, ShortestPathSolution>(testcase(2, i, {
            entry("vertixCount", i * 10000),
            entry("maxEdgeCount", i * 50000),
            entry("maxWeight", 10000)
        }));
    }
    
    temp.execute(std::thread::hardware_concurrency());
    return 0;
}