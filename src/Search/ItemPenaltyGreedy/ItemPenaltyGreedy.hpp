#ifndef ITEMPENALTYGREEDY_H_
#define ITEMPENALTYGREEDY_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

/*それぞれのアイテム間のペナルティを改善する貪欲法*/
class ItemPenaltyGreedy : public Search {
public:
    ItemPenaltyGreedy(const std::vector<Item>& items, double init_weight, int param, const Solution& solution);
    std::unique_ptr<Solution> operator()(const Solution& current_solution) override;               //貪欲法で新たな解を生成
};

#endif