#ifndef SWAPNEIGHBORHOOD_H_
#define SWAPNEIGHBORHOOD_H_

#include <Search.hpp>
#include <vector>

struct Item;
class Solution;
class Destroy;

/*swap‹ß–T*/
class SwapNeighborhood : public Search {
public:
    using Search::Search;
    std::unique_ptr<Solution> operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;   //swap‹ß–T‚ğ’Tõ
};

#endif