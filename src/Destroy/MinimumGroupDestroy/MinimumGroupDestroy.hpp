#ifndef MINIMUMGROUPDESTROY_H_
#define MINIMUMGROUPDESTROY_H_

#include <Destroy.hpp>
#include <Group.hpp>
#include <Solution.hpp>
#include <vector>

struct Item;

/*value関連を除く評価値の和が良くないグループを選び除去する破壊法*/
class MinimumGroupDestroy : public Destroy {
public:
    /*コンストラクタ*/
    MinimumGroupDestroy(const std::vector<Item>& items, int destroy_num, double init_weight, int param, const Solution& solution) : Destroy(items, init_weight, param) {
        set_destroy_num(destroy_num, solution);
    }
    std::vector<const Item*> operator()(Solution& solution) const override;                         //破壊法を実行
    void add_destroy_num(int add_num, const Solution& solution) override;                           //破壊するグループ数をadd_num分増加
    void set_destroy_num(int set_num, const Solution& solution) override;                           //破壊するグループ数を設定
};

/*破壊するグループ数をadd_num分増加*/
inline void MinimumGroupDestroy::add_destroy_num(int add_num, const Solution& solution) {
    set_destroy_num(destroy_num + add_num, solution);
}

/*破壊するグループ数を設定*/
inline void MinimumGroupDestroy::set_destroy_num(int set_num, const Solution& solution) {
    if (set_num > solution.get_valid_groups().size() * 0.4) {
        destroy_num = solution.get_valid_groups().size() * 0.4;
    }
    else if (set_num < 2) {
        destroy_num = 2;
    }
    else {
        destroy_num = set_num;
    }
}

#endif