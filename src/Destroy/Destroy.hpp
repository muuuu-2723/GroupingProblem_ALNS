#ifndef DESTROY_H_
#define DESTROY_H_

#include <Weight.hpp>
#include <vector>

class Person;
class Solution;

class Destroy {
protected:
    std::vector<Person>& persons;
    Weight weight;
public:
    Destroy(std::vector<Person>& persons, int param) : persons(persons), weight(param) {}
    virtual void operator()(Solution& solution) {}
    virtual void update_weight(double score) final;
    virtual double get_weight() const final;
};

inline void Destroy::update_weight(double score) {
    weight.update(score);
}

inline double Destroy::get_weight() const {
    return weight.get_weight();
}

#include "RandomDestroy\RandomDestroy.hpp"
#include "RandomGroupDestroy\RandomGroupDestroy.hpp"
#include "MinimumDestroy\MinimumDestroy.hpp"
#include "MinimumGroupDestroy\MinimumGroupDestroy.hpp"

#endif