#ifndef SWAPNEIGHBORHOOD_H_
#define SWAPNEIGHBORHOOD_H_

#include <Search.hpp>
#include <vector>

struct Item;
class Solution;
class Destroy;

/*swapãﬂñT*/
class SwapNeighborhood : public Search {
public:
    SwapNeighborhood(const std::vector<Item>& items, double init_weight, int param, const Solution& solution);
    std::unique_ptr<Solution> operator()(const Solution& current_solution) override;                            //swapãﬂñTÇíTçı
    void reset_destroy_num(const Solution& solution) override {}
    void update_destroy_num(const Solution& solution) override {}
    void update_weight(double score) override;
    const Destroy& select_destroy() override;
};

inline void SwapNeighborhood::update_weight(double score) {
    weight.update(score);
}

inline const Destroy& SwapNeighborhood::select_destroy() {
    return *destructions[0];
}

#endif