#ifndef WEIGHTPENALTYGREEDY_H_
#define WEIGHTPENALTYGREEDY_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

/*各グループの重みを上下限にできるだけ収める貪欲法*/
class WeightPenaltyGreedy : public Search {
public:
    WeightPenaltyGreedy(const std::vector<Item>& items, double init_weight, int param, const Solution& solution);
    std::unique_ptr<Solution> operator()(const Solution& current_solution) override;           //貪欲法で新たな解を生成
};

#endif