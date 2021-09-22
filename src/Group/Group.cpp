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
vector<int> Group::upper_num;
vector<int> Group::lower_num;

Group::Group(int group_id) {
    id = group_id;
    member_num = 0;
    num_ratio.assign(upper_num.size(), 0);
    sum_values.assign()
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
    num_ratio[item.get_category_num()] -= 1;
    for (size_t i = 0; i < sum_values.size(); ++i) {
        sum_values[i] -= item.values[i];
    }
    
    member_id.erase(itr);
}

/*1人をメンバーに加える*/
void Group::add_member(const Item& item) {
    member_id.push_back(item.id);
    member_num++;
    num_ratio[item.get_category_num()] += 1;
    for (size_t i = 0; i < sum_values.size(); ++i) {
        sum_values[i] += item.values[i];
    }
}

int Group::group_score_distance(const Item& item) const {
    int sd = 0;
    for (auto m_id : member_id) {
        sd += item.score_distances[m_id];
    }
    return sd;
}

int Group::group_scode_distance_all(const vector<Item>& items) const {
    int sd = 0;

    for (auto itr1 = member_id.begin(), end = member_id.end(); itr1 != end; ++itr1) {
        for (auto itr2 = std::next(itr1); itr2 != end; ++itr2) {
            sd += items[*itr1].score_distances[*itr2];
        }
    }

    return sd;
}

/*ある人とこの班の人との関係値と回数の和*/
int Group::group_relation(const Item& item) const {
    int r = 0;
    for (auto m_id : member_id) {
        r += item.relations[m_id];
        r -= item.times[m_id];
    }
    return r;
}

/*この班の関係値と回数の和*/
int Group::group_relation_all(const vector<Item>& items) const {
    int result = 0;

    for (auto itr1 = member_id.begin(), end = member_id.end(); itr1 != end; ++itr1) {
        for (auto itr2 = std::next(itr1); itr2 != end; ++itr2) {
            result += items[*itr1].relations[*itr2];
            result -= items[*itr1].times[*itr2];
        }
    }

    return result;
}

/*ある人を班員に加えた時のペナルティ変化量*/
int Group::add_penalty(int category_upper, int category_lower, const Item& item) const {
    using std::ceil;
    int penalty = 0;
    //班員数に対するペナルティ
    if (member_num + 1 != ceil((double)Item::N / N) || member_num != Item::N / N) {
        if (member_num >= ceil((double)Item::N / N)) {
            penalty++;
        }
        else {
            penalty--;
        }
    }

    //人数比に対するペナルティ
    if (num_ratio[item.year - 1][item.gender][item.campus] + 1 != category_upper ||
        num_ratio[item.year - 1][item.gender][item.campus] != category_lower) {
        
        if (num_ratio[item.year - 1][item.gender][item.campus] >= category_upper) {
            penalty++;
        }
        else {
            penalty--;
        }
    }
    return penalty;
}

/*班員pを削除した時のペナルティ変化量*/
int Group::erase_penalty(int category_upper, int category_lower, const Item& item) const {
    using std::ceil;
    int penalty = 0;
    //班員数に対するペナルティ
    if (member_num != ceil((double)Item::N / N) || member_num - 1 != Item::N / N) {
        if (member_num - 1 >= ceil((double)Item::N / N)) {
            penalty--;
        }
        else {
            penalty++;
        }
    }

    //人数比に対するペナルティ
    if (num_ratio[item.year - 1][item.gender][item.campus] != category_upper ||
        num_ratio[item.year - 1][item.gender][item.campus] - 1 != category_lower) {

        if (num_ratio[item.year - 1][item.gender][item.campus] - 1 >= category_upper) {
            penalty--;
        }
        else {
            penalty++;
        }
    }
    return penalty;
}

int Group::diff_penalty(const vector<const Item*>& add, const vector<const Item*>& erase) const {
    
    using std::ceil;
    using std::abs;
    int penalty = 0;
    int diff_num = add.size() - erase.size();
    if (diff_num != 0) {
        penalty = std::min(abs((int)ceil((double)Item::N / N) - (member_num + diff_num)), abs(Item::N / N - (member_num + diff_num)));
        penalty -= std::min(abs((int)ceil((double)Item::N / N) - member_num), abs(Item::N / N - member_num));
    }

    vector<vector<vector<int>>> diff_nums(upper_num.size(), vector<vector<int>>(upper_num[0].size(), vector<int>(upper_num[0][0].size(), 0)));
    //std::unordered_set<std::tuple<int, int, int>> change_category;
    for (const auto& item : add) {
        diff_nums[item->year - 1][item->gender][item->campus]++;
        //change_category.insert(std::make_tuple(item.year, item.gender, item.campus));
    }
    for (const auto& item : erase) {
        diff_nums[item->year - 1][item->gender][item->campus]--;
        //change_category.insert(std::make_tuple(item.year, item.gender, item.campus));
    }

    for (int i = 0; i < upper_num.size(); i++) {
        for (int j = 0; j < upper_num[i].size(); j++) {
            for (int k = 0; k < upper_num[i][j].size(); k++) {
                if (diff_nums[i][j][k] != 0) {
                    int now_penalty = std::min(abs(upper_num[i][j][k] - num_ratio[i][j][k]), abs(lower_num[i][j][k] - num_ratio[i][j][k]));
                    int next_penalty = std::min(abs(upper_num[i][j][k] - (num_ratio[i][j][k] + diff_nums[i][j][k])), 
                                                abs(lower_num[i][j][k] - (num_ratio[i][j][k] + diff_nums[i][j][k])));
                    penalty += (next_penalty - now_penalty);
                }
            }
        }
    }
    /*for (auto&& category : change_category) {
        int i = std::get<0>(category) - 1, j = std::get<1>(category), k = std::get<2>(category);
        int now_penalty = std::min(abs(upper[i][j][k] - num_ratio[i][j][k]), abs(lower[i][j][k] - num_ratio[i][j][k]));
        int next_penalty = std::min(abs(upper[i][j][k] - (num_ratio[i][j][k] + diff_nums[i][j][k])),
                                    abs(lower[i][j][k] - (num_ratio[i][j][k] + diff_nums[i][j][k])));
        penalty += (next_penalty - now_penalty);
    }*/
    return penalty;
}

int Group::calc_penalty() const {
    int penalty = 0;
    
    //班員数のペナルティ
    if (member_num > std::ceil((double)Item::N / N)) {
        penalty += (member_num - std::ceil((double)Item::N / N));
    }
    else if (member_num < Item::N / N) {
        penalty += (Item::N / N - member_num);
    }

    //人数比のペナルティ
    for (int i = 0; i < upper_num.size(); i++) {
        for (int j = 0; j < upper_num[i].size(); j++) {
            for (int k = 0; k < upper_num[i][j].size(); k++) {
                if (num_ratio[i][j][k] >= upper_num[i][j][k]) {
                    penalty += (num_ratio[i][j][k] - upper_num[i][j][k]);
                }
                else {
                    penalty += (lower_num[i][j][k] - num_ratio[i][j][k]);
                }
            }
        }
    }
    return penalty;
}

void Group::set_upper_and_lower(const vector<Item>& items) {
    int max_year = 1, max_genger = 0, max_campus = 0;
    for (const auto& item : items) {
        if (max_year < item.year) max_year = item.year;
        if (max_genger < item.gender) max_genger = item.gender;
        if (max_campus < item.campus) max_campus = item.campus;
    }
    upper_num.assign(max_year, vector<vector<int>>(max_genger + 1, vector<int>(max_campus + 1, 0)));
    lower_num.assign(max_year, vector<vector<int>>(max_genger + 1, vector<int>(max_campus + 1, 0)));

    for (const auto& item : items) {
        upper_num[item.year - 1][item.gender][item.campus]++;
        lower_num[item.year - 1][item.gender][item.campus]++;
    }
    for (int i = 0; i < upper_num.size(); ++i) {
        for (int j = 0; j < upper_num[i].size(); ++j) {
            for (int k = 0; k < upper_num[i][j].size(); ++k) {
                upper_num[i][j][k] = std::ceil((double)upper_num[i][j][k] / N);
                lower_num[i][j][k] = lower_num[i][j][k] / N;
            }
        }
    }
}

std::ostream& operator<<(std::ostream& out, const Group& g) {
    out << std::setw((int)std::log10(Group::N) + 1) << g.id << ":";
    for (const auto& member : g.member_id) {
        out << std::setw((int)std::log10(Item::N) + 2) << member;
    }
    return out;
}