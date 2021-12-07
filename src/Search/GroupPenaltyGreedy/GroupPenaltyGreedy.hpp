#ifndef GROUPPENALTYGREEDY_H_
#define GROUPPENALTYGREEDY_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

/*アイテムとグループのペナルティを改善する貪欲法*/
class GroupPenaltyGreedy : public Search {
public:
    GroupPenaltyGreedy(const std::vector<Item>& items, double init_weight, int param, const Solution& solution);
    std::unique_ptr<Solution> operator()(const Solution& current_solution) override;           //貪欲法で新たな解を生成
};

#endif