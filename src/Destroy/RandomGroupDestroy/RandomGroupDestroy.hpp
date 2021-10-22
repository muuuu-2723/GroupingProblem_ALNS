#ifndef RANDOMGROUPDESTROY_H_
#define RANDOMGROUPDESTROY_H_

#include <Destroy.hpp>
#include <Group.hpp>
#include <vector>

struct Item;
class Solution;

/*�����_���ɐ��O���[�v�I��, �����ɏ�������A�C�e������������j��@*/
class RandomGroupDestroy : public Destroy {
private:
    int destroy_num;                                //�j�󂷂�O���[�v��
public:
    /*�R���X�g���N�^*/
    RandomGroupDestroy(const std::vector<Item>& items, int destroy_num, double init_weight, int param) : Destroy(items, init_weight, param), destroy_num(destroy_num) {}
    void operator()(Solution& solution) override;                       //�j��@�����s
    void add_destroy_num(int add_num);                                  //�j�󂷂�O���[�v����add_num������
    void set_destroy_num(int set_num);                                  //�j�󂷂�O���[�v����ݒ�
    int get_destroy_num() const;                                        //�j�󂷂�O���[�v�����擾
};

/*�j�󂷂�O���[�v����add_num������*/
inline void RandomGroupDestroy::add_destroy_num(int add_num) {
    if (destroy_num + add_num < Group::N && destroy_num + add_num > 0) {
        destroy_num += add_num;
    }
}

/*�j�󂷂�O���[�v����ݒ�*/
inline void RandomGroupDestroy::set_destroy_num(int set_num) {
    if (set_num < Group::N && set_num > 0) {
        destroy_num = set_num;
    }
}

/*�j�󂷂�O���[�v�����擾*/
inline int RandomGroupDestroy::get_destroy_num() const {
    return destroy_num;
}

#endif