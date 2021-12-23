#ifndef RANDOMGROUPDESTROY_H_
#define RANDOMGROUPDESTROY_H_

#include <Destroy.hpp>
#include <Group.hpp>
#include <Solution.hpp>
#include <vector>

struct Item;

/*ランダムに数グループ選び, そこに所属するアイテムを除去する破壊法*/
class RandomGroupDestroy : public Destroy {
private:
    double upper_destroy_ratio;
public:
    /*コンストラクタ*/
    RandomGroupDestroy(const std::vector<Item>& items, int destroy_num, double init_weight, int param, const Solution& solution, double upper_destroy_ratio = 0.4) : Destroy(items, init_weight, param) {
        this->upper_destroy_ratio = upper_destroy_ratio;
        set_destroy_num(destroy_num, solution);
    }
    std::vector<const Item*> operator()(Solution& solution) const override;                                 //破壊法を実行
    void add_destroy_num(int add_num, const Solution& solution) override;                                   //破壊するグループ数をadd_num分増加
    void set_destroy_num(int set_num, const Solution& solution) override;                                   //破壊するグループ数を設定
};

/*破壊するグループ数をadd_num分増加*/
inline void RandomGroupDestroy::add_destroy_num(int add_num, const Solution& solution) {
    set_destroy_num(destroy_num + add_num, solution);
}

/*破壊するグループ数を設定*/
inline void RandomGroupDestroy::set_destroy_num(int set_num, const Solution& solution) {
    if (set_num > solution.get_valid_groups().size() * upper_destroy_ratio) {
        destroy_num = solution.get_valid_groups().size() * upper_destroy_ratio;
    }
    else if (set_num < 2) {
        destroy_num = 2;
    }
    else {
        destroy_num = set_num;
    }
}

#endif