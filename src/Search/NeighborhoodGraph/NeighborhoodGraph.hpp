#ifndef NEIGHBORHOODGRAPH_H_
#define NEIGHBORHOODGRAPH_H_

#include <vector>
#include <memory>
#include <Search.hpp>

struct Person;
class Solution;
class Destroy;

struct Vertex {
    int id;
    Person& person;
    Vertex(int id, Person& person) : id(id), person(person) {}
};


struct Edge {
    int target;
    double weight;
    Edge(int target, double weight) : target(target), weight(weight) {}
};

class NeighborhoodGraph : public Search {
private:
    std::vector<Vertex> vertices;
    std::vector<std::vector<Edge>> graph;
    std::vector<Person> dummy_persons;
    void set_edge(Solution& solution);
public:
    NeighborhoodGraph(std::vector<Person>& persons, int param);
    Solution operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;
};

#endif