#ifndef RELATIONGREEDY_H_
#define RELATIONGREEDY_H_

#include <Search.hpp>
#include <vector>
#include <memory>

class Solution;
class Destroy;

class RelationGreedy : public Search {
private:
    
public:
    using Search::Search;
    Solution operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;
};

#endif