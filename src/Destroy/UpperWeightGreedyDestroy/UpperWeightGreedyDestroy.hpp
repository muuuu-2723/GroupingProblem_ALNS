#ifndef UPPERWEIGHTGREEDYDESTROY_H_
#define UPPERWEIGHTGREEDYDESTROY_H_

#include <Destroy.hpp>
#include <Group.hpp>
#include <vector>

struct Item;
class Solution;

class UpperWeightGreedyDestroy : public Destroy {
private:
    double upper_destroy_ratio;
public:
    UpperWeightGreedyDestroy(const std::vector<Item>& items, int destroy_num, double init_weight, int param, const Solution& solution, double upper_destroy_ratio = 0.4) : Destroy(items, init_weight, param) {
        this->upper_destroy_ratio = upper_destroy_ratio;
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