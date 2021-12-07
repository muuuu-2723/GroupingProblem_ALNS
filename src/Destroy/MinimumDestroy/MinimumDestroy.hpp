#ifndef MINIMUMDESTROY_H_
#define MINIMUMDESTROY_H_

#include <Destroy.hpp>
#include <vector>
#include <iostream>

struct Item;
class Solution;

/*value関連を除く評価値が良くないアイテムを選び除去する破壊法*/
class MinimumDestroy : public Destroy {
public:
    /*コンストラクタ*/
    MinimumDestroy(const std::vector<Item>& items, int destroy_num, double init_weight, int param, const Solution& solution) : Destroy(items, init_weight, param) {
        set_destroy_num(destroy_num, solution);
    }
    std::vector<const Item*> operator()(Solution& solution) const override;                             //破壊法を実行
    void add_destroy_num(int add_num, const Solution& solution) override;                               //除去するアイテム数をadd_num分増加
    void set_destroy_num(int set_num, const Solution& solution) override;                               //除去するアイテム数を設定
};

/*除去するアイテム数をadd_num分増加*/
inline void MinimumDestroy::add_destroy_num(int add_num, const Solution& solution) {
    set_destroy_num(destroy_num + add_num, solution);
}

/*除去するアイテム数を設定*/
inline void MinimumDestroy::set_destroy_num(int set_num, const Solution& solution) {
    if (set_num > items.size() * 0.4) {
        destroy_num = items.size() * 0.4;
    }
    else if (set_num < 3) {
        destroy_num = 3;
    }
    else {
        destroy_num = set_num;
    }
}

#endif