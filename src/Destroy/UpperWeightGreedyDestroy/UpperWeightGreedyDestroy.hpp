#ifndef UPPERWEIGHTGREEDYDESTROY_H_
#define UPPERWEIGHTGREEDYDESTROY_H_

#include <Destroy.hpp>
#include <Group.hpp>
#include <vector>

struct Item;
class Solution;

class UpperWeightGreedyDestroy : public Destroy {
public:
    UpperWeightGreedyDestroy(const std::vector<Item>& items, int destroy_num, double init_weight, int param, const Solution& solution) : Destroy(items, init_weight, param) {
        set_destroy_num(destroy_num, solution);
    }
    std::vector<const Item*> operator()(Solution& solution) const override;
    void add_destroy_num(int add_num, const Solution& solution) override;
    void set_destroy_num(int set_num, const Solution& solution) override;
};

inline void UpperWeightGreedyDestroy::add_destroy_num(int add_num, const Solution& solution) {
    set_destroy_num(destroy_num + add_num, solution);
}

inline void UpperWeightGreedyDestroy::set_destroy_num(int set_num, const Solution& solution) {
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