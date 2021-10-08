#ifndef VALUESUMGREEDY_H_
#define VALUESUMGREEDY_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

/*各グループのvalueの合計を平滑化する貪欲法*/
class ValueSumGreedy : public Search {
public:
    using Search::Search;
    Solution operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;           //貪欲法で新たな解を生成
};

#endif