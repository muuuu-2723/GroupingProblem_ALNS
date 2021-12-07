#ifndef DECREASEGROUP_H_
#define DECREASEGROUP_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

class DecreaseGroup : public Search {
public:
    DecreaseGroup(const std::vector<Item>& items, double init_weight, int param, const Solution& solution);
    std::unique_ptr<Solution> operator()(const Solution& current_solution) override;
};

#endif