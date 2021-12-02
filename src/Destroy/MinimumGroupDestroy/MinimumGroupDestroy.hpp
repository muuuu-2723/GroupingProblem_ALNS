#ifndef MINIMUMGROUPDESTROY_H_
#define MINIMUMGROUPDESTROY_H_

#include <Destroy.hpp>
#include <Group.hpp>
#include <vector>

struct Item;
class Solution;

/*value�֘A�������]���l�̘a���ǂ��Ȃ��O���[�v��I�я�������j��@*/
class MinimumGroupDestroy : public Destroy {
private:
    int destroy_num;                                //�j�󂷂�O���[�v��
public:
    /*�R���X�g���N�^*/
    MinimumGroupDestroy(const std::vector<Item>& items, int destroy_num, double init_weight, int param) : Destroy(items, init_weight, param), destroy_num(destroy_num) {}
    std::vector<const Item*> operator()(Solution& solution) const override;                         //�j��@�����s
    void add_destroy_num(int add_num);                                                              //�j�󂷂�O���[�v����add_num������
    void set_destroy_num(int set_num);                                                              //�j�󂷂�O���[�v����ݒ�
    int get_destroy_num() const;                                                                    //�j�󂷂�O���[�v�����擾
};

/*�j�󂷂�O���[�v����add_num������*/
inline void MinimumGroupDestroy::add_destroy_num(int add_num) {
    if (destroy_num + add_num < Group::N && destroy_num + add_num > 0) {
        destroy_num += add_num;
    }
}

/*�j�󂷂�O���[�v����ݒ�*/
inline void MinimumGroupDestroy::set_destroy_num(int set_num) {
    if (set_num < Group::N && set_num > 0) {
        destroy_num = set_num;
    }
}

/*�j�󂷂�O���[�v�����擾*/
inline int MinimumGroupDestroy::get_destroy_num() const {
    return destroy_num;
}

#endif