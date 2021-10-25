#ifndef SHIFTNEIGHBORHOOD_H_
#define SHIFTNEIGHBORHOOD_H_

#include <Search.hpp>
#include <vector>
#include <memory>

struct Item;
class Solution;
class Destroy;

/*shiftãﬂñT*/
class ShiftNeighborhood : public Search {
public:
    using Search::Search;
    std::unique_ptr<Solution> operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;   //shiftãﬂñTÇíTçı
};

#endif