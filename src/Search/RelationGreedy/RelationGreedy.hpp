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
    using Search::Search;
    Solution operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;           //�×~�@�ŐV���ȉ��𐶐�
};

#endif