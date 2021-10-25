#ifndef VALUEDIVERSITYGREEDY_H_
#define VALUEDIVERSITYGREEDY_H_

#include <Search.hpp>
#include <memory>

class Solution;
class Destroy;

/*�O���[�v����value�̂΂����傫�������×~�@*/
class ValueDiversityGreedy : public Search {
public:
    using Search::Search;
    std::unique_ptr<Solution> operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;           //�×~�@�ŐV���ȉ��𐶐�
};

#endif