#ifndef MINIMUMDESTROY_H_
#define MINIMUMDESTROY_H_

#include <Destroy.hpp>
#include <vector>
#include <iostream>

struct Item;
class Solution;

/*value�֘A�������]���l���ǂ��Ȃ��A�C�e����I�я�������j��@*/
class MinimumDestroy : public Destroy {
private:
    double upper_destroy_ratio;
public:
    /*�R���X�g���N�^*/
    MinimumDestroy(const std::vector<Item>& items, int destroy_num, double init_weight, int param, const Solution& solution, double upper_destroy_ratio = 0.4) : Destroy(items, init_weight, param) {
        this->upper_destroy_ratio = upper_destroy_ratio;
        set_destroy_num(destroy_num, solution);
    }
    std::vector<const Item*> operator()(Solution& solution) const override;                             //�j��@�����s
    void add_destroy_num(int add_num, const Solution& solution) override;                               //��������A�C�e������add_num������
    void set_destroy_num(int set_num, const Solution& solution) override;                               //��������A�C�e������ݒ�
};

/*��������A�C�e������add_num������*/
inline void MinimumDestroy::add_destroy_num(int add_num, const Solution& solution) {
    set_destroy_num(destroy_num + add_num, solution);
}

/*��������A�C�e������ݒ�*/
inline void MinimumDestroy::set_destroy_num(int set_num, const Solution& solution) {
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