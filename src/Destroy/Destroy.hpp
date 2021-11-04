#ifndef DESTROY_H_
#define DESTROY_H_

#include <Weight.hpp>
#include <vector>

class Item;
class Solution;

/*破壊法の基底クラス*/
class Destroy {
protected:
    const std::vector<Item>& items;                                     //グループ分けするアイテムの集合
    Weight weight;                                                      //選択確率を決める重み
public:
    /*コンストラクタ*/
    Destroy(const std::vector<Item>& items, double init_weight, int param) : items(items), weight(init_weight ,param) {}
    /*解を破壊*/
    virtual void operator()(Solution& solution) {}
    virtual void update_weight(double score) final;
    virtual double get_weight() const final;
};

inline void Destroy::update_weight(double score) {
    weight.update(score);
}

inline double Destroy::get_weight() const {
    return weight.get_weight();
}

#include "RandomDestroy\RandomDestroy.hpp"
#include "RandomGroupDestroy\RandomGroupDestroy.hpp"
#include "MinimumDestroy\MinimumDestroy.hpp"
#include "MinimumGroupDestroy\MinimumGroupDestroy.hpp"
#include "UpperWeightGreedyDestroy\UpperWeightGreedyDestroy.hpp"

#endif