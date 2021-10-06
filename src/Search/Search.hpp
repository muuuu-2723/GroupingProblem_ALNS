#ifndef SEARCH_H_
#define SEARCH_H_

#include <Weight.hpp>
#include <vector>
#include <memory>

struct Item;
class Solution;
class Destroy;

/*�ߖT�ƍ\�z�@�̊��N���X*/
class Search {
protected:
    const std::vector<Item>& items;                             //�O���[�v��������A�C�e���̏W��
    Weight weight;                                              //�I���m�������߂�d��
public:
    /*�R���X�g���N�^*/
    Search(const std::vector<Item>& items, double init_weight, int param) : items(items), weight(init_weight, param) {}
    /*�V���ȉ��𐶐�*/
    virtual Solution operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) = 0;
    virtual void update_weight(double score) final;
    virtual double get_weight() const final;
};

/*�d�݂̍X�V*/
inline void Search::update_weight(double score) {
    weight.update(score);
}

/*�d�݂̎擾*/
inline double Search::get_weight() const {
    return weight.get_weight();
}

#include "NeighborhoodGraph\NeighborhoodGraph.hpp"
#include "ShiftNeighborhood\ShiftNeighborhood.hpp"
#include "SwapNeighborhood\SwapNeighborhood.hpp"
#include "GroupPenaltyGreedy\GroupPenaltyGreedy.hpp"
#include "ItemPenaltyGreedy\ItemPenaltyGreedy.hpp"
#include "WeightPenaltyGreedy\WeightPenaltyGreedy.hpp"
#include "RelationGreedy\RelationGreedy.hpp"
#include "ValueDiversityGreedy\ValueDiversityGreedy.hpp"

#endif