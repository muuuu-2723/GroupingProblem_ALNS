#ifndef VALUEAVERAGEGREEDY_H_
#define VALUEAVERAGEGREEDY_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

/*�e�O���[�v��value�̕��ς𕽊��������×~�@*/
class ValueAverageGreedy : public Search {
public:
    using Search::Search;
    Solution operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;           //�×~�@�ŐV���ȉ��𐶐�
};

#endif