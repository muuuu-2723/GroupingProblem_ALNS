#ifndef RANDOMGROUPDESTROY_H_
#define RANDOMGROUPDESTROY_H_

#include <Destroy.hpp>
#include <Group.hpp>
#include <vector>

struct Person;
class Solution;

class RandomGroupDestroy : public Destroy {
private:
    int destroy_num;
    std::vector<int> shuffle_group_ids;
public:
    RandomGroupDestroy(std::vector<Person>& persons, int destroy_num, int param);
    void operator()(Solution& solution) override;
    void add_destroy_num(int add_num);
    void set_destroy_num(int set_num);
    int get_destroy_num() const;
};

inline void RandomGroupDestroy::add_destroy_num(int add_num) {
    if (destroy_num + add_num < Group::N && destroy_num + add_num > 0) {
        destroy_num += add_num;
    }
}

inline void RandomGroupDestroy::set_destroy_num(int set_num) {
    if (set_num < Group::N && set_num > 0) {
        destroy_num = set_num;
    }
}

inline int RandomGroupDestroy::get_destroy_num() const {
    return destroy_num;
}

#endif