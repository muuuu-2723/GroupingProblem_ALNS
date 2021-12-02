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
    int destroy_num;                                        //��������A�C�e����
    std::vector<const Item*> target_items;                  //�I�΂��\��������A�C�e��
public:
    RandomDestroy(const std::vector<Item>& items, int destroy_num, double init_weight, int param);  //�R���X�g���N�^
    std::vector<const Item*> operator()(Solution& solution) const override;                         //�j��@�����s
    void add_destroy_num(int add_num);                                                              //��������A�C�e������add_num������
    void set_destroy_num(int set_num);                                                              //��������A�C�e������ݒ�
    int get_destroy_num() const;                                                                    //��������A�C�e�������擾
};

/*��������A�C�e������add_num������*/
inline void RandomDestroy::add_destroy_num(int add_num) {
    if (destroy_num + add_num < items.size() * 0.4 && destroy_num + add_num > 2) {
        destroy_num += add_num;
        //std::cerr << "random:" << destroy_num << std::endl;
    }
}

/*��������A�C�e������ݒ�*/
inline void RandomDestroy::set_destroy_num(int set_num) {
    if (set_num < items.size() * 0.4 && set_num > 2) {
        destroy_num = set_num;
        //std::cerr << "random:" << destroy_num << std::endl;
    }
}

/*��������A�C�e�������擾*/
inline int RandomDestroy::get_destroy_num() const {
    return destroy_num;
}

#endif