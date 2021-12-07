#ifndef RELATIONGREEDY_H_
#define RELATIONGREEDY_H_

#include <Search.hpp>
#include <vector>
#include <memory>

class Solution;
class Destroy;

/*それぞれのアイテム間の関係値とアイテムとグループの間の関係値の貪欲法*/
class RelationGreedy : public Search {
public:
    RelationGreedy(const std::vector<Item>& items, double init_weight, int param, const Solution& solution);
    std::unique_ptr<Solution> operator()(const Solution& current_solution) override;           //貪欲法で新たな解を生成
};

#endif