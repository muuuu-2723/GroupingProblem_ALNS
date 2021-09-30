#ifndef SCOREGREEDY_H_
#define SCOREGREEDY_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

class ScoreGreedy : public Search {
public:
    using Search::Search;
    Solution operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;
};

#endif