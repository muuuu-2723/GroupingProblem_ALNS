#ifndef ITEMPENALTYGREEDY_H_
#define ITEMPENALTYGREEDY_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

/*���ꂼ��̃A�C�e���Ԃ̃y�i���e�B�����P�����×~�@*/
class ItemPenaltyGreedy : public Search {
public:
    using Search::Search;
    Solution operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy) override;               //�×~�@�ŐV���ȉ��𐶐�
};

#endif