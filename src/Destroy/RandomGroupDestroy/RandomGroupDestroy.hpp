#ifndef RANDOMGROUPDESTROY_H_
#define RANDOMGROUPDESTROY_H_

#include <Destroy.hpp>
#include <Group.hpp>
#include <Solution.hpp>
#include <vector>

struct Item;

/*�����_���ɐ��O���[�v�I��, �����ɏ�������A�C�e������������j��@*/
class RandomGroupDestroy : public Destroy {
public:
    /*�R���X�g���N�^*/
    RandomGroupDestroy(const std::vector<Item>& items, int destroy_num, double init_weight, int param, const Solution& solution) : Destroy(items, init_weight, param) {
        set_destroy_num(destroy_num, solution);
    }
    std::vector<const Item*> operator()(Solution& solution) const override;                                 //�j��@�����s
    void add_destroy_num(int add_num, const Solution& solution) override;                                   //�j�󂷂�O���[�v����add_num������
    void set_destroy_num(int set_num, const Solution& solution) override;                                   //�j�󂷂�O���[�v����ݒ�
};

/*�j�󂷂�O���[�v����add_num������*/
inline void RandomGroupDestroy::add_destroy_num(int add_num, const Solution& solution) {
    set_destroy_num(destroy_num + add_num, solution);
}

/*�j�󂷂�O���[�v����ݒ�*/
inline void RandomGroupDestroy::set_destroy_num(int set_num, const Solution& solution) {
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