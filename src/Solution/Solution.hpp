#ifndef SOLUTION_H_
#define SOLUTION_H_

#include <Group.hpp>
#include <Item.hpp>
#include <Input.hpp>
#include <vector>
#include <optional>
#include <iostream>
#include <tuple>
#include <cmath>
#include <memory>
#include <list>
#include <bitset>

/*�A�C�e���̈ړ����*/
struct MoveItem {
    const Item& item;       //�ړ�����A�C�e��
    int source;             //�ړ����̃O���[�vid
    int destination;        //�ړ���̃O���[�vid

    /*�R���X�g���N�^*/
    MoveItem(const Item& item, int source, int destination) : item(item), source(source), destination(destination) {}
};

/*�O���[�v�������Ǘ������*/
class Solution {
private:
    std::vector<Group> groups;                                                                  //�O���[�v�̏W��
    std::list<std::unique_ptr<const Group>> valid_groups;                                       //���ݎg���Ă���O���[�v�̎Q�ƏW��
    std::vector<int> item_group_ids;                                                            //���ꂼ��̃A�C�e������������O���[�vid
    double relation;                                                                            //���̃O���[�v�����̊֌W�l
    double penalty;                                                                             //���̃O���[�v�����̃y�i���e�B
    double ave_balance;                                                                         //�e�O���[�v��value�̕��ϒl�̂΂��
    double sum_balance;                                                                         //�e�O���[�v��value�̍��v�̂΂��
    std::vector<std::vector<std::optional<std::vector<double>>>> each_group_item_relation;      //���ꂼ��̃O���[�v�ɑ΂���item_relation each_group_item_relation[�A�C�e��][�O���[�v].value()[type]
    std::vector<std::vector<std::optional<int>>> each_group_item_penalty;                       //���ꂼ��̃O���[�v�ɑ΂���item_penalty each_group_item_penalty[�A�C�e��][�O���[�v]
    std::vector<double> aves;                                                                   //value�̃A�C�e���P�ʂł̕���
    std::vector<double> sum_values;                                                             //value�̍��v
    Input::Opt opt;                                                                             //�ŏ������ő剻��
    std::vector<double> item_relation_params;                                                   //���ꂼ��̃A�C�e���Ԃ̊֌W�l�̃p�����[�^
    std::vector<double> group_relation_params;                                                  //�A�C�e���ƃO���[�v�̊Ԃ̊֌W�l�̃p�����[�^
    std::vector<double> value_ave_params;                                                       //�e�O���[�v��value�̕��ϒl�̂΂���̃p�����[�^
    std::vector<double> value_sum_params;                                                       //�e�O���[�v��value�̍��v�̂΂���̃p�����[�^
    int penalty_param;                                                                          //�y�i���e�B�̃p�����[�^
    double group_num_param;                                                                     //�O���[�v���̃p�����[�^
    double constant;                                                                            //�ړI�֐��̒萔
    std::bitset<8> eval_flags;                                                                  //�e�]���l���v�Z����K�v�����邩���Ǘ�����t���O

    void move_processing(const std::vector<MoveItem>& move_items, const std::tuple<double, double, double, double, int>& diff);     //�ړ�����
    void set_eval_value(int relation, int penalty, double ave_balance, double sum_balance);                                         //�]���l�̌��ƂȂ�relation, penalty, ave_balance, sum_balamce�̐ݒ�

public:
    enum EvalIdx {
        WEIGHT_PENA, ITEM_PENA, GROUP_PENA, ITEM_R, GROUP_R, VALUE_AVE, VALUE_SUM, GROUP_NUM
    };
    Solution(const Input& input);                                                                                                   //�R���X�g���N�^
    double get_eval_value() const;                                                                                                  //�]���l���擾
    double calc_diff_eval(const std::tuple<double, double, double, double, int>& diff) const;                                       //�ω��ʂɑ΂���]���l���v�Z
    const std::vector<double>& get_ave() const;                                                                                     //value�̃A�C�e���P�ʂł̕��ς��擾
    const std::vector<double>& get_sum_values() const;                                                                              //value�̍��v���擾
    const std::vector<double>& get_each_group_item_relation(const Item& item, int group_id);                                        //each_group_item_relation�̒l���擾, �Ȃ���Όv�Z���Ď擾
    int get_each_group_item_penalty(const Item& item, int group_id);                                                                //each_group_item_penalty�̒l���擾, �Ȃ���Όv�Z���Ď擾
    int get_group_id(const Item& item) const;                                                                                       //�A�C�e���̏�������O���[�vid���擾
    auto get_groups_range() const -> const std::pair<std::vector<Group>::const_iterator, std::vector<Group>::const_iterator>;       //�_�~�[�O���[�v������(Group::N)�O���[�v���擾
    auto get_valid_groups() const -> const std::list<std::unique_ptr<const Group>>&;                                                //���ݎg���Ă���O���[�v���擾
    const std::vector<Group>& get_groups() const;                                                                                   //�_�~�[�O���[�v���܂ނ��ׂẴO���[�v���擾
    const Group& get_dummy_group() const;                                                                                           //�_�~�[�O���[�v���擾
    double evaluation_all(const std::vector<Item>& items);                                                                          //���݂̉�(�O���[�v����)��]��
    auto evaluation_diff(const std::vector<MoveItem>& move_items) -> std::tuple<double, double, double, double, int>;               //�]���l�̕ω��ʂ��v�Z
    auto evaluation_shift(const Item& item, int group_id) -> std::tuple<double, double, double, double, int>;                       //shift�ړ����̕]���l�̕ω��ʂ��v�Z
    auto evaluation_swap(const Item& item1, const Item& item2) -> std::tuple<double, double, double, double, int>;                  //swap�ړ����̕]���l�̕ω��ʂ��v�Z
    bool shift_check(const Item& item, int group_id);                                                                               //shift�ړ����邩�ǂ����𒲍���, �K�v�ɉ����Ĉړ�����
    bool swap_check(const Item& item1, const Item& item2);                                                                          //swap�ړ����邩�ǂ����𒲍���, �K�v�ɉ����Ĉړ�����
    bool move_check(const std::vector<MoveItem>& move_items);                                                                       //move_items�Ɋ�Â��Ĉړ����邩�ǂ����𒲍���, �K�v�ɉ����Ĉړ�����
    void move(const std::vector<MoveItem>& move_items);                                                                             //move_items�Ɋ�Â��Ĉړ�����
    double get_relation() const;                                                                                                    //�֌W�l���擾
    double get_penalty() const;                                                                                                     //�y�i���e�B���擾
    double get_ave_balance() const;                                                                                                 //�e�O���[�v��value�̕��ϒl�̂΂�����擾
    double get_sum_balance() const;                                                                                                 //�e�O���[�v��value�̍��v�̂΂�����擾
    const std::vector<double>& get_group_relation_params() const;                                                                   //�A�C�e���ƃO���[�v�Ԃ̊֌W�l�̃p�����[�^���擾
    const std::bitset<8>& get_eval_flags() const;                                                                                   //eval_flags���擾

    friend std::ostream& operator<<(std::ostream&, const Solution&);                                                                //���̏o�͗p
};

/*�]���l�̌��ƂȂ�relation, penalty, ave_balance, sum_balamce�̐ݒ�*/
inline void Solution::set_eval_value(int relation, int penalty, double ave_balance, double sum_balance) {
    this->relation = relation;
    this->penalty = penalty;
    this->ave_balance = ave_balance;
    this->sum_balance = sum_balance;
}

/*�]���l���擾*/
inline double Solution::get_eval_value() const {
    return relation - penalty * penalty_param + ave_balance + sum_balance + valid_groups.size() * group_num_param + constant;
}

/*�ω��ʂɑ΂���]���l���v�Z*/
inline double Solution::calc_diff_eval(const std::tuple<double, double, double, double, int>& diff) const {
    return std::get<0>(diff) - std::get<1>(diff) * penalty_param + std::get<2>(diff) + std::get<3>(diff) + std::get<4>(diff) * group_num_param;
}

/*value�̃A�C�e���P�ʂł̕��ς��擾*/
inline const std::vector<double>& Solution::get_ave() const {
    return aves;
}

/*value�̍��v���擾*/
inline const std::vector<double>& Solution::get_sum_values() const {
    return sum_values;
}

/*�A�C�e���̏�������O���[�vid���擾*/
inline int Solution::get_group_id(const Item& item) const {
    return item_group_ids[item.id];
}

/*�_�~�[�O���[�v������(Group::N)�O���[�v���擾*/
inline auto Solution::get_groups_range() const -> const std::pair<std::vector<Group>::const_iterator, std::vector<Group>::const_iterator> {
    return {groups.cbegin(), groups.cbegin() + Group::N};
}

/*���ݎg���Ă���O���[�v���擾*/
inline auto Solution::get_valid_groups() const -> const std::list<std::unique_ptr<const Group>>& {
    return valid_groups;
}

/*�_�~�[�O���[�v���܂ނ��ׂẴO���[�v���擾*/
inline const std::vector<Group>& Solution::get_groups() const {
    return groups;
}

/*�_�~�[�O���[�v���擾*/
inline const Group& Solution::get_dummy_group() const {
    return groups[Group::N];
}

/*�֌W�l���擾*/
inline double Solution::get_relation() const {
    return relation;
}

/*�y�i���e�B���擾*/
inline double Solution::get_penalty() const {
    return penalty;
}

/*�e�O���[�v��value�̕��ϒl�̂΂�����擾*/
inline double Solution::get_ave_balance() const {
    return ave_balance;
}

/*�e�O���[�v��value�̍��v�̂΂�����擾*/
inline double Solution::get_sum_balance() const {
    return sum_balance;
}

/*�A�C�e���ƃO���[�v�Ԃ̊֌W�l�̃p�����[�^���擾*/
inline const std::vector<double>& Solution::get_group_relation_params() const {
    return group_relation_params;
}

/*eval_flags���擾*/
inline const std::bitset<8>& Solution::get_eval_flags() const {
    return eval_flags;
}

#endif