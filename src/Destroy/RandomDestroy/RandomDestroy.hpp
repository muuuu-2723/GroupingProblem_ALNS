#ifndef RANDOMDESTROY_H_
#define RANDOMDESTROY_H_

#include <Destroy.hpp>
#include <vector>
#include <iostream>

struct Item;
class Solution;

/*ランダムに数アイテムを除去する破壊法*/
class RandomDestroy : public Destroy {
private:
    int destroy_num;                                        //除去するアイテム数
    std::vector<const Item*> target_items;                  //選ばれる可能性があるアイテム
public:
    RandomDestroy(const std::vector<Item>& items, int destroy_num, double init_weight, int param);  //コンストラクタ
    std::vector<const Item*> operator()(Solution& solution) const override;                         //破壊法を実行
    void add_destroy_num(int add_num);                                                              //除去するアイテム数をadd_num分増加
    void set_destroy_num(int set_num);                                                              //除去するアイテム数を設定
    int get_destroy_num() const;                                                                    //除去するアイテム数を取得
};

/*除去するアイテム数をadd_num分増加*/
inline void RandomDestroy::add_destroy_num(int add_num) {
    if (destroy_num + add_num < items.size() * 0.4 && destroy_num + add_num > 2) {
        destroy_num += add_num;
        //std::cerr << "random:" << destroy_num << std::endl;
    }
}

/*除去するアイテム数を設定*/
inline void RandomDestroy::set_destroy_num(int set_num) {
    if (set_num < items.size() * 0.4 && set_num > 2) {
        destroy_num = set_num;
        //std::cerr << "random:" << destroy_num << std::endl;
    }
}

/*除去するアイテム数を取得*/
inline int RandomDestroy::get_destroy_num() const {
    return destroy_num;
}

#endif