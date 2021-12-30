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

struct EvalVals {
    double relation;
    double penalty;
    double ave_balance;
    double sum_balance;
    double group_cost;
    int group_num;

    EvalVals() : relation(0), penalty(0), ave_balance(0), sum_balance(0), group_cost(0), group_num(0) {}

    double get_value(double penalty_param, double constant) const {
        return relation - penalty * penalty_param + ave_balance / group_num + sum_balance / group_num + group_cost + constant;
    }

    EvalVals& operator+=(const EvalVals& ev) {
        this->relation += ev.relation;
        this->penalty += ev.penalty;
        this->ave_balance += ev.ave_balance;
        this->sum_balance += ev.sum_balance;
        this->group_cost += ev.group_cost;
        this->relation = std::abs(this->relation) < 1e-10 ? 0 : this->relation;
        this->penalty = std::abs(this->penalty) < 1e-10 ? 0 : this->penalty;
        this->ave_balance = std::abs(this->ave_balance) < 1e-10 ? 0 : this->ave_balance;
        this->sum_balance = std::abs(this->sum_balance) < 1e-10 ? 0 : this->sum_balance;
        this->group_num += ev.group_num;
        return *this;
    }
};

/*�O���[�v�������Ǘ������*/
class Solution {
private:
    std::vector<Group> groups;                                                                  //�O���[�v�̏W��
    std::list<const Group*> valid_groups;                                                       //���ݎg���Ă���O���[�v�̎Q�ƏW��
    std::vector<int> item_group_ids;                                                            //���ꂼ��̃A�C�e������������O���[�vid
    EvalVals eval;
    std::vector<std::vector<std::optional<double>>> each_group_item_relation;                   //���ꂼ��̃O���[�v�ɑ΂���item_relation each_group_item_relation[�A�C�e��][�O���[�v].value()
    std::vector<std::vector<std::optional<int>>> each_group_item_penalty;                       //���ꂼ��̃O���[�v�ɑ΂���item_penalty each_group_item_penalty[�A�C�e��][�O���[�v]
    std::vector<double> aves;                                                                   //value�̃A�C�e���P�ʂł̕���
    std::vector<double> sum_values;                                                             //value�̍��v
    Input::Opt opt;                                                                             //�ŏ������ő剻��
    std::vector<double> value_ave_params;                                                       //�e�O���[�v��value�̕��ϒl�̂΂���̃p�����[�^
    std::vector<double> value_sum_params;                                                       //�e�O���[�v��value�̍��v�̂΂���̃p�����[�^
    int penalty_param;                                                                          //�y�i���e�B�̃p�����[�^
    std::vector<double> group_cost;                                                             //�O���[�v���̃p�����[�^
    double constant;                                                                            //�ړI�֐��̒萔
    std::bitset<8> eval_flags;                                                                  //�e�]���l���v�Z����K�v�����邩���Ǘ�����t���O

    void move_processing(const std::vector<MoveItem>& move_items, const EvalVals& diff);     //�ړ�����
    std::vector<std::vector<int>> item_times;
    std::vector<std::vector<int>> group_times;
    std::vector<std::vector<bool>> same_group;

public:
    enum EvalIdx {
        WEIGHT_PENA, ITEM_PENA, GROUP_PENA, ITEM_R, GROUP_R, VALUE_AVE, VALUE_SUM, GROUP_COST
    };
    Solution(const Input& input);                                                                                                   //�R���X�g���N�^
    Solution(const Solution& s);
    Solution(Solution&& s) = delete;
    Solution& operator=(const Solution& s);
    Solution& operator=(Solution&& s) = delete;
    double get_eval_value() const;                                                                                                  //�]���l���擾
    double calc_diff_eval(const EvalVals& diff) const;                                    //�ω��ʂɑ΂���]���l���v�Z
    const std::vector<double>& get_ave() const;                                                                                     //value�̃A�C�e���P�ʂł̕��ς��擾
    const std::vector<double>& get_sum_values() const;                                                                              //value�̍��v���擾
    double get_each_group_item_relation(const Item& item, int group_id);                                                            //each_group_item_relation�̒l���擾, �Ȃ���Όv�Z���Ď擾
    int get_each_group_item_penalty(const Item& item, int group_id);                                                                //each_group_item_penalty�̒l���擾, �Ȃ���Όv�Z���Ď擾
    int get_group_id(const Item& item) const;                                                                                       //�A�C�e���̏�������O���[�vid���擾
    auto get_groups_range() const -> const std::pair<std::vector<Group>::const_iterator, std::vector<Group>::const_iterator>;       //�_�~�[�O���[�v������(Group::N)�O���[�v���擾
    auto get_valid_groups() const -> const std::list<const Group*>&;                                                                //���ݎg���Ă���O���[�v���擾
    const std::vector<Group>& get_groups() const;                                                                                   //�_�~�[�O���[�v���܂ނ��ׂẴO���[�v���擾
    const Group& get_dummy_group() const;                                                                                           //�_�~�[�O���[�v���擾
    double evaluation_all(const std::vector<Item>& items);                                                                          //���݂̉�(�O���[�v����)��]��
    EvalVals evaluation_diff(const std::vector<MoveItem>& move_items);            //�]���l�̕ω��ʂ��v�Z
    EvalVals evaluation_shift(const Item& item, int group_id);                    //shift�ړ����̕]���l�̕ω��ʂ��v�Z
    EvalVals evaluation_swap(const Item& item1, const Item& item2);               //swap�ړ����̕]���l�̕ω��ʂ��v�Z
    bool shift_check(const Item& item, int group_id);                                                                               //shift�ړ����邩�ǂ����𒲍���, �K�v�ɉ����Ĉړ�����
    bool swap_check(const Item& item1, const Item& item2);                                                                          //swap�ړ����邩�ǂ����𒲍���, �K�v�ɉ����Ĉړ�����
    bool move_check(const std::vector<MoveItem>& move_items);                                                                       //move_items�Ɋ�Â��Ĉړ����邩�ǂ����𒲍���, �K�v�ɉ����Ĉړ�����
    void move(const std::vector<MoveItem>& move_items);                                                                             //move_items�Ɋ�Â��Ĉړ�����
    void shift_move(const Item& item, int group_id);
    void swap_move(const Item& item1, const Item& item2);
    double get_relation() const;                                                                                                    //�֌W�l���擾
    double get_penalty() const;                                                                                                     //�y�i���e�B���擾
    double get_ave_balance() const;                                                                                                 //�e�O���[�v��value�̕��ϒl�̂΂�����擾
    double get_sum_balance() const;                                                                                                 //�e�O���[�v��value�̍��v�̂΂�����擾
    double get_sum_group_cost() const;
    const std::vector<double>& get_group_relation_params() const;                                                                   //�A�C�e���ƃO���[�v�Ԃ̊֌W�l�̃p�����[�^���擾
    const std::bitset<8>& get_eval_flags() const;                                                                                   //eval_flags���擾
    const std::vector<std::vector<bool>>& get_same_group() const;

    friend std::ostream& operator<<(std::ostream&, const Solution&);                                                                //���̏o�͗p
    auto get_item_times() const -> const std::vector<std::vector<int>>&;
    auto get_group_times() const -> const std::vector<std::vector<int>>&;
    void counter();
    int debug_same_group() const;
};

inline Solution& Solution::operator=(const Solution& s) {
    std::cout << "�R�s�[������Z�q" << std::endl;
    groups = s.groups;
    item_group_ids = s.item_group_ids;
    eval = s.eval;
    each_group_item_relation = s.each_group_item_relation;
    each_group_item_penalty = s.each_group_item_penalty;
    aves = s.aves;
    sum_values = s.sum_values;
    opt = s.opt;
    value_ave_params = s.value_ave_params;
    value_sum_params = s.value_sum_params;
    penalty_param = s.penalty_param;
    group_cost = s.group_cost;
    constant = s.constant;
    eval_flags = s.eval_flags;
    item_times = s.item_times;
    group_times = s.group_times;
    same_group = s.same_group;

    valid_groups.clear();
    for (auto&& g_ptr : s.valid_groups) {
        valid_groups.push_back(&groups[g_ptr->get_id()]);
    }

    return *this;
}

EvalVals operator+(const EvalVals& ev1, const EvalVals& ev2);

int distance(const Solution& s1, const Solution& s2);

/*�]���l���擾*/
inline double Solution::get_eval_value() const {
    return eval.get_value(penalty_param, constant);
}

/*�ω��ʂɑ΂���]���l���v�Z*/
inline double Solution::calc_diff_eval(const EvalVals& diff) const {
    return (eval + diff).get_value(penalty_param, constant) - eval.get_value(penalty_param, constant);
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
inline auto Solution::get_valid_groups() const -> const std::list<const Group*>& {
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
    return eval.relation;
}

/*�y�i���e�B���擾*/
inline double Solution::get_penalty() const {
    return eval.penalty;
}

/*�e�O���[�v��value�̕��ϒl�̂΂�����擾*/
inline double Solution::get_ave_balance() const {
    return eval.ave_balance;
}

/*�e�O���[�v��value�̍��v�̂΂�����擾*/
inline double Solution::get_sum_balance() const {
    return eval.sum_balance;
}

inline double Solution::get_sum_group_cost() const {
    return eval.group_cost;
}

/*eval_flags���擾*/
inline const std::bitset<8>& Solution::get_eval_flags() const {
    return eval_flags;
}

inline const std::vector<std::vector<bool>>& Solution::get_same_group() const {
    return same_group;
}

inline auto Solution::get_item_times() const -> const std::vector<std::vector<int>>& {
    return item_times;
}

inline auto Solution::get_group_times() const -> const std::vector<std::vector<int>>& {
    return group_times;
}

inline int Solution::debug_same_group() const {
    int result = 0;
    for (size_t i = 0, size = same_group.size(); i < size; ++i) {
        for (size_t j = i + 1; j < size; ++j) {
            if (same_group[i][j]) ++result;
        }
    }
    return result;
}

#endif