#ifndef VALUEAVERAGEGREEDY_H_
#define VALUEAVERAGEGREEDY_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

/*各グループのvalueの平均を平滑化する貪欲法*/
class ValueAverageGreedy : public Search {
public:
    using Search::Search;
    Solution operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;           //貪欲法で新たな解を生成
};

#endif