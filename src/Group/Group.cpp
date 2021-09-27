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

/*namespace std {
    template<>
    class hash<tuple<int, int, int>> {
    public:
        size_t operator()(const tuple<int, int, int>& t) const {
            return hash<int>()(get<0>(t)) ^ hash<int>()(get<1>(t)) ^ hash<int>()(get<2>(t));
        }
    };
}*/

int Group::N = 0;
vector<double> Group::upper_weight;
vector<double> Group::lower_weight;

Group::Group(int group_id) {
    id = group_id;
    member_num = 0;
    sum_weight.assign(Item::w_size, 0);
    sum_values.assign(Item::v_size, 0);
}

Group::Group(Item& leader, vector<Item>& members, int group_id)
: Group(group_id) {
    add_member(leader);
    for (auto&& member : members) {
        add_member(member);
    }
}

/*メンバー1人を削除*/
void Group::erase_member(const Item& item) {
    if (item.predefined_group != -1) {
        cerr << "erase_error:リーダー削除, id = " << item.id << endl;
        std::exit(1);
    }
    auto itr = std::find(member_id.begin(), member_id.end(), item.id);
    if (itr == member_id.end()) {
        cerr << "erase_error:メンバー未発見, id = " << item.id << endl;
        std::exit(1);
    }

    member_num--;
    for (size_t i = 0; i < Item::w_size; i++) {
        sum_weight[i] -= item.weight[i];
    }
    for (size_t i = 0; i < Item::v_size; ++i) {
        sum_values[i] -= item.values[i];
    }
    
    member_id.erase(itr);
}

/*1人をメンバーに加える*/
void Group::add_member(const Item& item) {
    member_id.push_back(item.id);
    member_num++;
    for (size_t i = 0; i < Item::w_size; i++) {
        sum_weight[i] += item.weight[i];
    }
    for (size_t i = 0; i < Item::v_size; ++i) {
        sum_values[i] += item.values[i];
    }
}

vector<double> Group::item_relation(const Item& item) const {
    vector<double> result(item.item_relations.size(), 0);
    for (size_t i = 0, size = result.size(); i < size; i++) {
        for (const auto& m_id : member_id) {
            result[i] += item.item_relations[i][m_id];
        }
    }
    return std::move(result);
}

vector<double> Group::sum_relation(const vector<Item>& items) const {
    vector<double> result(items[0].item_relations.size(), 0);
    for (size_t i = 0, size = result.size(); i < size; i++) {
        for (auto itr1 = member_id.begin(), end = member_id.end(); itr1 != end; ++itr1) {
            for (auto itr2 = std::next(itr1); itr2 != end; ++itr2) {
                result[i] += items[*itr1].item_relations[i][*itr2];
            }
        }
    }
    return std::move(result);
}

double Group::diff_penalty(const vector<const Item*>& add, const vector<const Item*>& erase) const {
    double penalty = 0;
    for (size_t i = 0; i < Item::w_size; i++) {
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

double Group::calc_penalty() const {
    double penalty = 0;
    for (size_t i = 0; i < Item::w_size; i++) {
        if (sum_weight[i] < lower_weight[i]) {
            penalty += lower_weight[i] - sum_weight[i];
        }
        else if (sum_weight[i] > upper_weight[i]) {
            penalty += sum_weight[i] - upper_weight[i];
        }
    }

    return penalty;
}

void Group::set_upper_and_lower(const vector<double>& upper, const vector<double>& lower) {
    if (upper.size() != Item::w_size || lower.size() != Item::w_size) {
        std::cerr << "weightの上下限のサイズエラー" << std::endl;
        std::exit(1);
    }
    upper_weight = upper;
    lower_weight = lower;
}

std::ostream& operator<<(std::ostream& out, const Group& g) {
    out << std::setw((int)std::log10(Group::N) + 1) << g.id << ":";
    for (const auto& member : g.member_id) {
        out << std::setw((int)std::log10(Item::N) + 2) << member;
    }
    return out;
}