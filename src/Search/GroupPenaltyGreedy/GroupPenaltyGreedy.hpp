#ifndef GROUPPENALTYGREEDY_H_
#define GROUPPENALTYGREEDY_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

/*�A�C�e���ƃO���[�v�̃y�i���e�B�����P�����×~�@*/
class GroupPenaltyGreedy : public Search {
public:
    using Search::Search;
    std::unique_ptr<Solution> operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;           //�×~�@�ŐV���ȉ��𐶐�
};

#endif