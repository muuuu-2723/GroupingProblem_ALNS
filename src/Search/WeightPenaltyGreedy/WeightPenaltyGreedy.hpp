#ifndef WEIGHTPENALTYGREEDY_H_
#define WEIGHTPENALTYGREEDY_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

/*�e�O���[�v�̏d�݂��㉺���ɂł��邾�����߂��×~�@*/
class WeightPenaltyGreedy : public Search {
public:
    using Search::Search;
    std::unique_ptr<Solution> operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;           //�×~�@�ŐV���ȉ��𐶐�
};

#endif