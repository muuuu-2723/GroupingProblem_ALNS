#ifndef RELATIONGREEDY_H_
#define RELATIONGREEDY_H_

#include <Search.hpp>
#include <vector>
#include <memory>

class Solution;
class Destroy;

/*���ꂼ��̃A�C�e���Ԃ̊֌W�l�ƃA�C�e���ƃO���[�v�̊Ԃ̊֌W�l���×~�@*/
class RelationGreedy : public Search {
public:
    RelationGreedy(const std::vector<Item>& items, double init_weight, int param, const Solution& solution);
    std::unique_ptr<Solution> operator()(const Solution& current_solution) override;           //�×~�@�ŐV���ȉ��𐶐�
};

#endif