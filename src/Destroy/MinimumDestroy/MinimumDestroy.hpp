#ifndef MINIMUMDESTROY_H_
#define MINIMUMDESTROY_H_

#include <Destroy.hpp>
#include <vector>
#include <iostream>

struct Person;
class Solution;

class MinimumDestroy : public Destroy {
private:
    int destroy_num;
public:
    MinimumDestroy(std::vector<Person>& persons, int destroy_num, int param) : Destroy(persons, param), destroy_num(destroy_num) {}
    void operator()(Solution& solution) override;
    void add_destroy_num(int add_num);
    void set_destroy_num(int set_num);
    int get_destroy_num() const;
};

inline void MinimumDestroy::add_destroy_num(int add_num) {
    if (destroy_num + add_num < persons.size() * 0.4 && destroy_num + add_num > 2) {
        destroy_num += add_num;
        std::cerr << "minimum:" << destroy_num << std::endl;
    }
}

inline void MinimumDestroy::set_destroy_num(int set_num) {
    if (set_num < persons.size() * 0.4 && set_num > 2) {
        destroy_num = set_num;
        std::cerr << "minimum:" << destroy_num << std::endl;
    }
}

inline int MinimumDestroy::get_destroy_num() const {
    return destroy_num;
}

#endif