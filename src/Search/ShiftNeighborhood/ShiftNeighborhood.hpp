#ifndef SHIFTNEIGHBORHOOD_H_
#define SHIFTNEIGHBORHOOD_H_

#include <Search.hpp>
#include <vector>
#include <memory>

struct Person;
class Solution;
class Destroy;

class ShiftNeighborhood : public Search {
private:
    std::vector<int> shuffle_member_ids;
    std::vector<int> shuffle_group_ids;
public:
    ShiftNeighborhood(std::vector<Person>& persons, int param);
    Solution operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;
};

#endif