#ifndef NEIGHBORHOODGRAPH_H_
#define NEIGHBORHOODGRAPH_H_

#include <vector>
#include <memory>
#include <Search.hpp>

struct Item;
class Solution;
class Destroy;

class NeighborhoodGraph : public Search {
    struct Vertex {
        int id;
        const Item& item;
        Vertex(int id, const Item& item) : id(id), item(item) {}
    };

    struct Edge {
        int target;
        double weight;
        Edge(int target, double weight) : target(target), weight(weight) {}
    };

private:
    std::vector<Vertex> vertices;
    std::vector<std::vector<Edge>> graph;
    std::vector<Item> dummy_items;
    void set_edge(Solution& solution);
public:
    NeighborhoodGraph(const std::vector<Item>& items, double init_weight, int param);
    Solution operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;
};

#endif