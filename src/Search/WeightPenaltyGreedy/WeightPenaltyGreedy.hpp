#ifndef WEIGHTPENALTYGREEDY_H_
#define WEIGHTPENALTYGREEDY_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

/*各グループの重みを上下限にできるだけ収める貪欲法*/
class WeightPenaltyGreedy : public Search {
public:
    using Search::Search;
    std::unique_ptr<Solution> operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;           //貪欲法で新たな解を生成
};

#endif