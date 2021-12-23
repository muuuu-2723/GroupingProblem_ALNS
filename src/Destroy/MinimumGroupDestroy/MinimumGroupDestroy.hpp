#ifndef MINIMUMGROUPDESTROY_H_
#define MINIMUMGROUPDESTROY_H_

#include <Destroy.hpp>
#include <Group.hpp>
#include <Solution.hpp>
#include <vector>

struct Item;

/*value�֘A�������]���l�̘a���ǂ��Ȃ��O���[�v��I�я�������j��@*/
class MinimumGroupDestroy : public Destroy {
private:
    double upper_destroy_ratio;
public:
    /*�R���X�g���N�^*/
    MinimumGroupDestroy(const std::vector<Item>& items, int destroy_num, double init_weight, int param, const Solution& solution, double upper_destroy_ratio = 0.4) : Destroy(items, init_weight, param) {
        this->upper_destroy_ratio = upper_destroy_ratio;
        set_destroy_num(destroy_num, solution);
    }
    std::vector<const Item*> operator()(Solution& solution) const override;                         //�j��@�����s
    void add_destroy_num(int add_num, const Solution& solution) override;                           //�j�󂷂�O���[�v����add_num������
    void set_destroy_num(int set_num, const Solution& solution) override;                           //�j�󂷂�O���[�v����ݒ�
};

/*�j�󂷂�O���[�v����add_num������*/
inline void MinimumGroupDestroy::add_destroy_num(int add_num, const Solution& solution) {
    set_destroy_num(destroy_num + add_num, solution);
}

/*�j�󂷂�O���[�v����ݒ�*/
inline void MinimumGroupDestroy::set_destroy_num(int set_num, const Solution& solution) {
    if (set_num > solution.get_valid_groups().size() * upper_destroy_ratio) {
        destroy_num = solution.get_valid_groups().size() * upper_destroy_ratio;
    }
    else if (set_num < 2) {
        destroy_num = 2;
    }
    else {
        destroy_num = set_num;
    }
}

#endif