#ifndef WEIGHTPENALTYGREEDY_H_
#define WEIGHTPENALTYGREEDY_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

/*�e�O���[�v�̏d�݂��㉺���ɂł��邾�����߂��×~�@*/
class WeightPenaltyGreedy : public Search {
public:
    WeightPenaltyGreedy(const std::vector<Item>& items, double init_weight, int param, const Solution& solution);
    std::unique_ptr<Solution> operator()(const Solution& current_solution) override;           //�×~�@�ŐV���ȉ��𐶐�
};

#endif