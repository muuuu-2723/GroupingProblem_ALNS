#ifndef VALUESUMGREEDY_H_
#define VALUESUMGREEDY_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

/*�e�O���[�v��value�̍��v�𕽊��������×~�@*/
class ValueSumGreedy : public Search {
public:
    ValueSumGreedy(const std::vector<Item>& items, double init_weight, int param, const Solution& solution);
    std::unique_ptr<Solution> operator()(const Solution& current_solution) override;           //�×~�@�ŐV���ȉ��𐶐�
};

#endif