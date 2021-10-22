#ifndef RANDOMGROUPDESTROY_H_
#define RANDOMGROUPDESTROY_H_

#include <Destroy.hpp>
#include <Group.hpp>
#include <vector>

struct Item;
class Solution;

/*ランダムに数グループ選び, そこに所属するアイテムを除去する破壊法*/
class RandomGroupDestroy : public Destroy {
private:
    int destroy_num;                                //破壊するグループ数
public:
    /*コンストラクタ*/
    RandomGroupDestroy(const std::vector<Item>& items, int destroy_num, double init_weight, int param) : Destroy(items, init_weight, param), destroy_num(destroy_num) {}
    void operator()(Solution& solution) override;                       //破壊法を実行
    void add_destroy_num(int add_num);                                  //破壊するグループ数をadd_num分増加
    void set_destroy_num(int set_num);                                  //破壊するグループ数を設定
    int get_destroy_num() const;                                        //破壊するグループ数を取得
};

/*破壊するグループ数をadd_num分増加*/
inline void RandomGroupDestroy::add_destroy_num(int add_num) {
    if (destroy_num + add_num < Group::N && destroy_num + add_num > 0) {
        destroy_num += add_num;
    }
}

/*破壊するグループ数を設定*/
inline void RandomGroupDestroy::set_destroy_num(int set_num) {
    if (set_num < Group::N && set_num > 0) {
        destroy_num = set_num;
    }
}

/*破壊するグループ数を取得*/
inline int RandomGroupDestroy::get_destroy_num() const {
    return destroy_num;
}

#endif