#ifndef SEARCH_H_
#define SEARCH_H_

#include <Weight.hpp>
#include <vector>
#include <memory>

struct Item;
class Solution;
class Destroy;

/*近傍と構築法の基底クラス*/
class Search {
protected:
    const std::vector<Item>& items;                             //グループ分けするアイテムの集合
    Weight weight;                                              //選択確率を決める重み
public:
    /*コンストラクタ*/
    Search(const std::vector<Item>& items, double init_weight, int param) : items(items), weight(init_weight, param) {}
    /*新たな解を生成*/
    virtual Solution operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) = 0;
    virtual void update_weight(double score) final;
    virtual double get_weight() const final;
};

/*重みの更新*/
inline void Search::update_weight(double score) {
    weight.update(score);
}

/*重みの取得*/
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