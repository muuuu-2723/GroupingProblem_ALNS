#ifndef ITEMPENALTYGREEDY_H_
#define ITEMPENALTYGREEDY_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

/*それぞれのアイテム間のペナルティを改善する貪欲法*/
class ItemPenaltyGreedy : public Search {
public:
    using Search::Search;
    Solution operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy) override;               //貪欲法で新たな解を生成
};

#endif