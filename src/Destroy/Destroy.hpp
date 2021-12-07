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
    int destroy_num;
public:
    /*コンストラクタ*/
    Destroy(const std::vector<Item>& items, double init_weight, int param) : items(items), weight(init_weight ,param), destroy_num(2) {}
    /*解を破壊*/
    virtual std::vector<const Item*> operator()(Solution& solution) const { return std::vector<const Item*>(); }
    virtual void update_weight(double score) final;
    virtual double get_weight() const final;
    virtual void add_destroy_num(int add_num, const Solution& solution) { destroy_num += add_num; }
    virtual void set_destroy_num(int set_num, const Solution& solution) { destroy_num = set_num; }
    virtual int get_destroy_num() const final;
};

inline void Destroy::update_weight(double score) {
    weight.update(score);
}

inline double Destroy::get_weight() const {
    return weight.get_weight();
}

inline int Destroy::get_destroy_num() const {
    return destroy_num;
}

#include "RandomDestroy\RandomDestroy.hpp"
#include "RandomGroupDestroy\RandomGroupDestroy.hpp"
#include "MinimumDestroy\MinimumDestroy.hpp"
#include "MinimumGroupDestroy\MinimumGroupDestroy.hpp"
#include "UpperWeightGreedyDestroy\UpperWeightGreedyDestroy.hpp"

#endif