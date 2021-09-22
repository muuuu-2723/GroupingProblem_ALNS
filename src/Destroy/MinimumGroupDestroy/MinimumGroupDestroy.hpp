#ifndef MINIMUMGROUPDESTROY_H_
#define MINIMUMGROUPDESTROY_H_

#include <Destroy.hpp>
#include <Group.hpp>
#include <vector>

struct Person;
class Solution;

class MinimumGroupDestroy : public Destroy {
private:
    int destroy_num;
public:
    MinimumGroupDestroy(std::vector<Person>& persons, int destroy_num, int param) : Destroy(persons, param), destroy_num(destroy_num) {}
    void operator()(Solution& solution) override;
    void add_destroy_num(int add_num);
    void set_destroy_num(int set_num);
    int get_destroy_num() const;
};

inline void MinimumGroupDestroy::add_destroy_num(int add_num) {
    if (destroy_num + add_num < Group::N && destroy_num + add_num > 0) {
        destroy_num += add_num;
    }
}

inline void MinimumGroupDestroy::set_destroy_num(int set_num) {
    if (set_num < Group::N && set_num > 0) {
        destroy_num = set_num;
    }
}

inline int MinimumGroupDestroy::get_destroy_num() const {
    return destroy_num;
}

#endif