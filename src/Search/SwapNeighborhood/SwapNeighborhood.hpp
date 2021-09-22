#ifndef SWAPNEIGHBORHOOD_H_
#define SWAPNEIGHBORHOOD_H_

#include <Search.hpp>
#include <vector>

struct Person;
class Solution;
class Destroy;

class SwapNeighborhood : public Search {
private:
    std::vector<int> shuffle_member_ids1;
    std::vector<int> shuffle_member_ids2;
public:
    SwapNeighborhood(std::vector<Person>& persons, int param);
    Solution operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;
};

#endif