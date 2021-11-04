#ifndef UPPERWEIGHTGREEDYDESTROY_H_
#define UPPERWEIGHTGREEDYDESTROY_H_

#include <Destroy.hpp>
#include <Group.hpp>
#include <vector>

struct Item;
class Solution;

class UpperWeightGreedyDestroy : public Destroy {
private:
    int destroy_num;
public:
    UpperWeightGreedyDestroy(const std::vector<Item>& items, int destroy_num, double init_weight, int param) : Destroy(items, init_weight, param), destroy_num(destroy_num) {}
    void operator()(Solution& solution) override;
    void add_destroy_num(int add_num);
    void set_destroy_num(int set_num);
    int get_destroy_num() const;
};

inline void UpperWeightGreedyDestroy::add_destroy_num(int add_num) {
    if (destroy_num + add_num < Group::N && destroy_num + add_num > 0) {
        destroy_num += add_num;
    }
}

inline void UpperWeightGreedyDestroy::set_destroy_num(int set_num) {
    if (set_num < Group::N && set_num > 0) {
        destroy_num = set_num;
    }
}

inline int UpperWeightGreedyDestroy::get_destroy_num() const {
    return destroy_num;
}

#endif