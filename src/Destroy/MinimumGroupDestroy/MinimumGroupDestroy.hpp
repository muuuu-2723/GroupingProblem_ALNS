#ifndef MINIMUMGROUPDESTROY_H_
#define MINIMUMGROUPDESTROY_H_

#include <Destroy.hpp>
#include <Group.hpp>
#include <vector>

struct Item;
class Solution;

/*value関連を除く評価値の和が良くないグループを選び除去する破壊法*/
class MinimumGroupDestroy : public Destroy {
private:
    int destroy_num;                                //破壊するグループ数
public:
    /*コンストラクタ*/
    MinimumGroupDestroy(const std::vector<Item>& items, int destroy_num, double init_weight, int param) : Destroy(items, init_weight, param), destroy_num(destroy_num) {}
    std::vector<const Item*> operator()(Solution& solution) const override;                         //破壊法を実行
    void add_destroy_num(int add_num);                                                              //破壊するグループ数をadd_num分増加
    void set_destroy_num(int set_num);                                                              //破壊するグループ数を設定
    int get_destroy_num() const;                                                                    //破壊するグループ数を取得
};

/*破壊するグループ数をadd_num分増加*/
inline void MinimumGroupDestroy::add_destroy_num(int add_num) {
    if (destroy_num + add_num < Group::N && destroy_num + add_num > 0) {
        destroy_num += add_num;
    }
}

/*破壊するグループ数を設定*/
inline void MinimumGroupDestroy::set_destroy_num(int set_num) {
    if (set_num < Group::N && set_num > 0) {
        destroy_num = set_num;
    }
}

/*破壊するグループ数を取得*/
inline int MinimumGroupDestroy::get_destroy_num() const {
    return destroy_num;
}

#endif