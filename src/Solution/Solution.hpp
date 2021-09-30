#ifndef SOLUTION_H_
#define SOLUTION_H_

#include <Group.hpp>
#include <Item.hpp>
#include <vector>
#include <optional>
#include <iostream>
#include <tuple>
#include <cmath>

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
    std::vector<int> item_group_ids;                                                            //���ꂼ��̃A�C�e������������O���[�vid
    double relation;                                                                            //���̃O���[�v�����̊֌W�l
    double penalty;                                                                             //���̃O���[�v�����̃y�i���e�B
    double ave_balance;                                                                         //�e�O���[�v��value�̕��ϒl�̂΂��
    double sum_balance;                                                                         //�e�O���[�v��value�̍��v�̂΂��
    std::vector<std::vector<std::optional<std::vector<double>>>> each_group_item_relation;      //���ꂼ��̃O���[�v�ɑ΂���item_relation each_group_item_relation[�A�C�e��][�O���[�v].value()[type]
    std::vector<std::vector<std::optional<int>>> each_group_item_penalty;                       //���ꂼ��̃O���[�v�ɑ΂���item_penalty each_group_item_penalty[�A�C�e��][�O���[�v]
    std::vector<double> aves;                                                                   //value�̃A�C�e���P�ʂł̕���
    std::vector<double> sum_values;                                                             //value�̍��v
    int relation_parameter;                                                                     //relation�̃p�����[�^
    int penalty_parameter;                                                                      //penalty�̃p�����[�^
    int ave_balance_parameter;                                                                  //ave_balance�̃p�����[�^
    int sum_balance_parameter;                                                                  //sum_balance�̃p�����[�^

    void move_processing(const std::vector<MoveItem>& move_items, const std::tuple<double, double, double, double>& diff);  //�ړ�����
    void set_eval_value(int relation, int penalty, double ave_balance, double sum_balance);                                 //�]���l�̌��ƂȂ�relation, penalty, ave_balance, sum_balamce�̐ݒ�

public:
    Solution(std::vector<Item>& items);                                                                                         //�R���X�g���N�^
    double get_eval_value() const;                                                                                              //�]���l���擾
    const std::vector<double>& get_ave() const;                                                                                 //value�̃A�C�e���P�ʂł̕��ς��擾
    const std::vector<double>& get_each_group_item_relation(const Item& item, int group_id);                                    //each_group_item_relation�̒l���擾, �Ȃ���Όv�Z���Ď擾
    int get_each_group_item_penalty(const Item& item, int group_id);                                                            //each_group_item_penalty�̒l���擾, �Ȃ���Όv�Z���Ď擾
    int get_group_id(const Item& item) const;                                                                                   //�A�C�e���̏�������O���[�vid���擾
    auto get_groups_range() const -> const std::pair<std::vector<Group>::const_iterator, std::vector<Group>::const_iterator>;   //�_�~�[�O���[�v������(Group::N)�O���[�v���擾
    const std::vector<Group>& get_groups() const;                                                                               //�_�~�[�O���[�v���܂ނ��ׂẴO���[�v���擾
    const Group& get_dummy_group() const;                                                                                       //�_�~�[�O���[�v���擾
    double evaluation_all(const std::vector<Item>& items);                                                                      //���݂̉�(�O���[�v����)��]��
    auto evaluation_diff(const std::vector<MoveItem>& move_items) -> std::tuple<double, double, double, double>;                //�]���l�̕ω��ʂ��v�Z
    auto evaluation_shift(const Item& item, int group_id) -> std::tuple<double, double, double, double>;                        //shift�ړ����̕]���l�̕ω��ʂ��v�Z
    auto evaluation_swap(const Item& item1, const Item& item2) -> std::tuple<double, double, double, double>;                   //swap�ړ����̕]���l�̕ω��ʂ��v�Z
    bool shift_check(const Item& item, int group_id);                                                                                 //shift�ړ����邩�ǂ����𒲍���, �K�v�ɉ����Ĉړ�����
    bool swap_check(const Item& item1, const Item& item2);                                                                                  //swap�ړ����邩�ǂ����𒲍���, �K�v�ɉ����Ĉړ�����
    bool move_check(const std::vector<MoveItem>& move_items);                                                                   //move_items�Ɋ�Â��Ĉړ����邩�ǂ����𒲍���, �K�v�ɉ����Ĉړ�����
    void move(const std::vector<MoveItem>& move_items);                                                                         //move_items�Ɋ�Â��Ĉړ�����
    double get_relation() const;                                                                                                //�֌W�l���擾
    double get_penalty() const;                                                                                                 //�y�i���e�B���擾
    double get_ave_balance() const;                                                                                             //�e�O���[�v��value�̕��ϒl�̂΂�����擾
    double get_sum_balance() const;                                                                                             //�e�O���[�v��value�̍��v�̂΂�����擾
    int get_penalty_parameter() const;                                                                                          //penalty�̃p�����[�^���擾
    int get_relation_parameter() const;                                                                                         //relation�̃p�����[�^���擾
    int get_ave_balance_parameter() const;                                                                                      //ave_balance�̃p�����[�^���擾
    int get_sum_balance_parameter() const;                                                                                      //sum_balance�̃p�����[�^���擾

    friend std::ostream& operator<<(std::ostream&, const Solution&);                                                            //���̏o�͗p
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
    return relation * relation_parameter - penalty * penalty_parameter + ave_balance * ave_balance_parameter + sum_balance * sum_balance_parameter;
}

/*value�̃A�C�e���P�ʂł̕��ς��擾*/
inline const std::vector<double>& Solution::get_ave() const {
    return aves;
}

/*�A�C�e���̏�������O���[�vid���擾*/
inline int Solution::get_group_id(const Item& item) const {
    return item_group_ids[item.id];
}

/*�_�~�[�O���[�v������(Group::N)�O���[�v���擾*/
inline auto Solution::get_groups_range() const -> const std::pair<std::vector<Group>::const_iterator, std::vector<Group>::const_iterator> {
    return {groups.cbegin(), groups.cbegin() + Group::N};
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

/*penalty�̃p�����[�^���擾*/
inline int Solution::get_penalty_parameter() const {
    return penalty_parameter;
}

/*relation�̃p�����[�^���擾*/
inline int Solution::get_relation_parameter() const {
    return relation_parameter;
}

/*ave_balance�̃p�����[�^���擾*/
inline int Solution::get_ave_balance_parameter() const {
    return ave_balance_parameter;
}

/*sum_balance�̃p�����[�^���擾*/
inline int Solution::get_sum_balance_parameter() const {
    return sum_balance_parameter;
}

#endif