#ifndef VALUEDIVERSITYGREEDY_H_
#define VALUEDIVERSITYGREEDY_H_

#include <Search.hpp>
#include <memory>
#include <vector>

class Solution;
class Destroy;
struct Item;

/*グループ内のvalueのばらつきを大きくする貪欲法*/
class ValueDiversityGreedy : public Search {
private:
    std::vector<int> value_types;
public:
    ValueDiversityGreedy(const std::vector<Item>& items, double init_weight, int param);
    std::unique_ptr<Solution> operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;           //貪欲法で新たな解を生成
};

#endif