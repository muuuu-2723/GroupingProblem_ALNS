#ifndef RANDOMDESTROY_H_
#define RANDOMDESTROY_H_

#include <Destroy.hpp>
#include <vector>
#include <iostream>

struct Item;
class Solution;

/*�����_���ɐ��A�C�e������������j��@*/
class RandomDestroy : public Destroy {
private:
    std::vector<const Item*> target_items;                  //�I�΂��\��������A�C�e��
    double upper_destroy_ratio;
public:
    RandomDestroy(const std::vector<Item>& items, int destroy_num, double init_weight, int param, const Solution& solution, double upper_destroy_ratio = 0.4);  //�R���X�g���N�^
    std::vector<const Item*> operator()(Solution& solution) const override;                         //�j��@�����s
    void add_destroy_num(int add_num, const Solution& solution) override;                           //��������A�C�e������add_num������
    void set_destroy_num(int set_num, const Solution& solution) override;                           //��������A�C�e������ݒ�
};

/*��������A�C�e������add_num������*/
inline void RandomDestroy::add_destroy_num(int add_num, const Solution& solution) {
    set_destroy_num(destroy_num + add_num, solution);
}

/*��������A�C�e������ݒ�*/
inline void RandomDestroy::set_destroy_num(int set_num, const Solution& solution) {
    if (set_num > items.size() * upper_destroy_ratio) {
        destroy_num = items.size() * upper_destroy_ratio;
    }
    else if (set_num < 3) {
        destroy_num = 3;
    }
    else {
        destroy_num = set_num;
    }
}

#endif