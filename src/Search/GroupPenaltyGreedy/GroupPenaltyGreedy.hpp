#ifndef GROUPPENALTYGREEDY_H_
#define GROUPPENALTYGREEDY_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

/*アイテムとグループのペナルティを改善する貪欲法*/
class GroupPenaltyGreedy : public Search {
public:
    using Search::Search;
    std::unique_ptr<Solution> operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;           //貪欲法で新たな解を生成
};

#endif