#ifndef VALUEAVERAGEGREEDY_H_
#define VALUEAVERAGEGREEDY_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

/*各グループのvalueの平均を平滑化する貪欲法*/
class ValueAverageGreedy : public Search {
public:
    ValueAverageGreedy(const std::vector<Item>& items, double init_weight, int param, const Solution& solution);
    std::unique_ptr<Solution> operator()(const Solution& current_solution) override;           //貪欲法で新たな解を生成
};

#endif