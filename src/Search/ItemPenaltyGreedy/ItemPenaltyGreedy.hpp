#ifndef ITEMPENALTYGREEDY
#define ITEMPENALTYGREEDY

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

class ItemPenaltyGreedy : public Search {
public:
    using Search::Search;
    Solution operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy) override;
};

#endif