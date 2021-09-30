#include "Group.hpp"
#include <Item.hpp>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <tuple>
#include <unordered_set>

using std::vector;
using std::list;
using std::cerr;
using std::endl;

int Group::N = 0;
vector<double> Group::upper_weight;
vector<double> Group::lower_weight;

/*�R���X�g���N�^*/
Group::Group(int group_id) {
    id = group_id;
    member_num = 0;
    sum_weight.assign(Item::w_size, 0);
    sum_values.assign(Item::v_size, 0);
}

/*�����o�[�w��̃R���X�g���N�^*/
Group::Group(Item& leader, vector<Item>& members, int group_id)
: Group(group_id) {
    add_member(leader);
    for (auto&& member : members) {
        add_member(member);
    }
}

/*��������A�C�e���̍폜*/
void Group::erase_member(const Item& item) {
    if (item.predefined_group != -1) {
        cerr << "erase_error:���[�_�[�폜, id = " << item.id << endl;
        std::exit(1);
    }
    auto itr = std::find(member_id.begin(), member_id.end(), item.id);
    if (itr == member_id.end()) {
        cerr << "erase_error:�����o�[������, id = " << item.id << endl;
        std::exit(1);
    }

    member_num--;
    for (size_t i = 0; i < Item::w_size; ++i) {
        sum_weight[i] -= item.weight[i];
    }
    for (size_t i = 0; i < Item::v_size; ++i) {
        sum_values[i] -= item.values[i];
    }
    
    member_id.erase(itr);
}

/*�V���ɃA�C�e����ǉ�����*/
void Group::add_member(const Item& item) {
    member_id.push_back(item.id);
    member_num++;
    for (size_t i = 0; i < Item::w_size; ++i) {
        sum_weight[i] += item.weight[i];
    }
    for (size_t i = 0; i < Item::v_size; ++i) {
        sum_values[i] += item.values[i];
    }
}

/*����A�C�e���Ƃ��̃O���[�v��item_relation*/
vector<double> Group::item_relation(const Item& item) const {
    vector<double> result(Item::item_r_size, 0);
    for (size_t i = 0; i < Item::item_r_size; ++i) {
        for (const auto& m_id : member_id) {
            result[i] += item.item_relations[i][m_id];
        }
    }
    return std::move(result);
}

/*���̃O���[�v��item_relation�̍��v*/
vector<double> Group::sum_item_relation(const vector<Item>& items) const {
    vector<double> result(Item::item_r_size, 0);
    for (size_t i = 0; i < Item::item_r_size; ++i) {
        for (auto itr1 = member_id.begin(), end = member_id.end(); itr1 != end; ++itr1) {
            for (auto itr2 = std::next(itr1); itr2 != end; ++itr2) {
                result[i] += items[*itr1].item_relations[i][*itr2];
            }
        }
    }
    return std::move(result);
}

/*���̃O���[�v��group_relation�̍��v*/
vector<double> Group::sum_group_relation(const vector<Item>& items) const {
    vector<double> result(Item::group_r_size, 0);
    for (size_t i = 0; i < Item::group_r_size; ++i) {
        for (const auto& m_id : member_id) {
            result[i] += items[m_id].group_relations[i][id];
        }
    }
    return std::move(result);
}

/*
 *weight_penalty�̕ω���
 *add�̃A�C�e���Q������, erase�̃A�C�e���Q���폜�����Ƃ���weight_penalty�̕ω���
 */
double Group::diff_weight_penalty(const vector<const Item*>& add, const vector<const Item*>& erase) const {
    double penalty = 0;
    for (size_t i = 0; i < Item::w_size; ++i) {
        double diff_weight = sum_weight[i];
        for (const auto& item : add) {
            diff_weight += item->weight[i];
        }
        for (const auto& item : erase) {
            diff_weight -= item->weight[i];
        }

        if (diff_weight < lower_weight[i]) {
            penalty += lower_weight[i] - diff_weight;
        }
        else if (diff_weight > upper_weight[i]) {
            penalty += diff_weight - upper_weight[i];
        }
    }
    
    return penalty;
}

/*���̃O���[�v��weight_penalty���v�Z*/
double Group::calc_weight_penalty() const {
    double penalty = 0;
    for (size_t i = 0; i < Item::w_size; ++i) {
        if (sum_weight[i] < lower_weight[i]) {
            penalty += lower_weight[i] - sum_weight[i];
        }
        else if (sum_weight[i] > upper_weight[i]) {
            penalty += sum_weight[i] - upper_weight[i];
        }
    }

    return penalty;
}

/*����A�C�e���Ƃ��̃O���[�v��item_penalty*/
int Group::calc_item_penalty(const Item& item) const {
    int penalty = 0;
    for (const auto& m_id : member_id) {
        penalty += item.item_penalty[m_id];
    }
    return penalty;
}

/*���̃O���[�v��item_penalty���v�Z*/
int Group::calc_sum_item_penalty(const vector<Item>& items) const {
    int penalty = 0;
    for (auto itr1 = member_id.begin(), end = member_id.end(); itr1 != end; ++itr1) {
        for (auto itr2 = std::next(itr1); itr2 != end; ++itr2) {
            penalty += items[*itr1].item_penalty[*itr2];
        }
    }
    return penalty;
}

/*���̃O���[�v��group_penalty���v�Z*/
int Group::calc_group_penalty(const vector<Item>& items) const {
    int penalty;
    for (const auto& m_id : member_id) {
        penalty += items[m_id].group_penalty[id];
    }
    return penalty;
}

/*weight�̏㉺����ݒ�*/
void Group::set_upper_and_lower(const vector<double>& upper, const vector<double>& lower) {
    if (upper.size() != Item::w_size || lower.size() != Item::w_size) {
        std::cerr << "weight�̏㉺���̃T�C�Y�G���[" << std::endl;
        std::exit(1);
    }
    upper_weight = upper;
    lower_weight = lower;
}

/*�O���[�v�̏o�͗p*/
std::ostream& operator<<(std::ostream& out, const Group& g) {
    out << std::setw((int)std::log10(Group::N) + 1) << g.id << ":";
    for (const auto& member : g.member_id) {
        out << std::setw((int)std::log10(Item::N) + 2) << member;
    }
    return out;
}