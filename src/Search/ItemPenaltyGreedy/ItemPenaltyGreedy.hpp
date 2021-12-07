#ifndef ITEMPENALTYGREEDY_H_
#define ITEMPENALTYGREEDY_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

/*���ꂼ��̃A�C�e���Ԃ̃y�i���e�B�����P�����×~�@*/
class ItemPenaltyGreedy : public Search {
public:
    ItemPenaltyGreedy(const std::vector<Item>& items, double init_weight, int param, const Solution& solution);
    std::unique_ptr<Solution> operator()(const Solution& current_solution) override;               //�×~�@�ŐV���ȉ��𐶐�
};

#endif