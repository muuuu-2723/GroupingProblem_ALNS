#ifndef PENALTYGREEDY_H_
#define PENALTYGREEDY_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

class PenaltyGreedy : public Search {
private:
    
public:
    using Search::Search;
    Solution operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;
};

#endif