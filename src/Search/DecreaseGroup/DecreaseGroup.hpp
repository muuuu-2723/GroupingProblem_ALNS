#ifndef DECREASEGROUP_H_
#define DECREASEGROUP_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

class DecreaseGroup : public Search {
public:
    using Search::Search;
    Solution operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;
};

#endif