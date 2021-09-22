#ifndef SEARCH_H_
#define SEARCH_H_

#include <Weight.hpp>
#include <vector>
#include <memory>

struct Person;
class Solution;
class Destroy;

class Search {
protected:
    std::vector<Person>& persons;
    Weight weight;
public:
    Search(std::vector<Person>& persons, int param) : persons(persons), weight(param) {}
    virtual Solution operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) = 0;
    virtual void update_weight(double score) final;
    virtual double get_weight() const final;
};

inline void Search::update_weight(double score) {
    weight.update(score);
}

inline double Search::get_weight() const {
    return weight.get_weight();
}

#include "NeighborhoodGraph\NeighborhoodGraph.hpp"
#include "ShiftNeighborhood\ShiftNeighborhood.hpp"
#include "SwapNeighborhood\SwapNeighborhood.hpp"
#include "PenaltyGreedy\PenaltyGreedy.hpp"
#include "RelationGreedy\RelationGreedy.hpp"
#include "ScoreGreedy\ScoreGreedy.hpp"

#endif