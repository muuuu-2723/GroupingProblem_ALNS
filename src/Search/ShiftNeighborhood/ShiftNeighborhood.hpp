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
    ShiftNeighborhood(const std::vector<Item>& items, double init_weight, int param, const Solution& solution);
    std::unique_ptr<Solution> operator()(const Solution& current_solution) override;   //shiftãﬂñTÇíTçı
    void reset_destroy_num(const Solution& solution) override {}
    void update_destroy_num(const Solution& solution, bool intensification) override {}
    void set_destroy_num(const Solution& solution, int set_num) override {}
    void update_weight(double score) override;
    const Destroy& select_destroy() override;
};

inline void ShiftNeighborhood::update_weight(double score) {
    weight.update(score);
}

inline const Destroy& ShiftNeighborhood::select_destroy() {
    return *destructions[0];
}

#endif