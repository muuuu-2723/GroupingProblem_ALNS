#ifndef RANDOMDESTROY_H_
#define RANDOMDESTROY_H_

#include <Destroy.hpp>
#include <vector>
#include <iostream>

struct Person;
class Solution;

class RandomDestroy : public Destroy {
private:
    int destroy_num;
    std::vector<int> shuffle_person_ids;
public:
    RandomDestroy(std::vector<Person>& persons, int destroy_num, int param);
    void operator()(Solution& solution) override;
    void add_destroy_num(int add_num);
    void set_destroy_num(int set_num);
    int get_destroy_num() const;
};

inline void RandomDestroy::add_destroy_num(int add_num) {
    if (destroy_num + add_num < persons.size() * 0.4 && destroy_num + add_num > 2) {
        destroy_num += add_num;
        std::cerr << "random:" << destroy_num << std::endl;
    }
}

inline void RandomDestroy::set_destroy_num(int set_num) {
    if (set_num < persons.size() * 0.4 && set_num > 2) {
        destroy_num = set_num;
        std::cerr << "random:" << destroy_num << std::endl;
    }
}

inline int RandomDestroy::get_destroy_num() const {
    return destroy_num;
}

#endif