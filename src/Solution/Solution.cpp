#include "Solution.hpp"
#include <Item.hpp>
#include <Group.hpp>
#include <MyRandom.hpp>
#include <Search.hpp>
#include <Destroy.hpp>

#include <vector>
#include <cmath>
#include <optional>
#include <iostream>
#include <numeric>
#include <cfloat>
#include <climits>
#include <tuple>
#include <iterator>
#include <memory>
#include <cassert>

using std::vector;

Solution::Solution(vector<Item>& items) {
    Group::set_upper_and_lower(items);
    penalty_parameter = 10000;
    relation_parameter = 40;
    score_parameter = 80;
    balance_parameter = 1;
    groups.clear();
    groups.reserve(Group::N + 1);
    for (size_t i = 0; i < Group::N + 1; ++i) {
        groups.push_back(Group(i));
    }
    each_group_item_relation.assign(Item::N, vector<std::optional<vector<double>>>(Group::N + 1, std::nullopt));
    each_group_item_penalty.assign(Item::N, vector<std::optional<int>>(Group::N + 1, std::nullopt));
    item_group_ids.resize(Item::N);
    aves.assign(Item::v_size, 0);
    sum_values.assign(Item::v_size, 0);

    //�ǒ��͎w��̔�, ����ȊO�̓����_���Ȕǂɒǉ�
    RandomInt<> rand_group(0, Group::N - 1);
    int cnt = 0;
    for (auto&& item : items) {
        if (item.predefined_group != -1) {
            groups[item.predefined_group].add_member(item);
            item_group_ids[item.id] = item.predefined_group;
        }
        else {
            groups[Group::N].add_member(item);
            item_group_ids[item.id] = Group::N;
        }
        for (size_t i = 0; i < Item::v_size; ++i) {
            sum_values[i] += item.values[i];
        }
    }

    for (size_t i = 0; i < Item::v_size; ++i) {
        aves[i] = sum_values[i] / Item::N;
    }
    

    //relation_greedy(items);
    

    //penalty_greedy(items);
    //score_greedy(items);
    RelationGreedy rg(items, 1);
    //PenaltyGreedy rg(items, 1);
    std::shared_ptr<Destroy> des = std::make_shared<Destroy>(items, 1);
    *this = std::move(rg(*this, des));
}

const vector<double>& Solution::get_each_group_item_relation(const Item& item, int group_id) {
    if (group_id >= Group::N) {
        each_group_item_relation[item.id][group_id] = vector<double>(Item::item_r_size, 0);
    }
    else if (!each_group_item_relation[item.id][group_id]) {
        each_group_item_relation[item.id][group_id] = groups[group_id].item_relation(item);
    }
    return each_group_item_relation[item.id][group_id].value();
}

int Solution::get_each_group_item_penalty(const Item& item, int group_id) {
    if (group_id >= Group::N) {
        each_group_item_penalty[item.id][group_id] = 0;
    }
    else if (!each_group_item_penalty[item.id][group_id]) {
        each_group_item_penalty[item.id][group_id] = groups[group_id].calc_item_penalty(item);
    }
    return each_group_item_penalty[item.id][group_id].value();
}

/*���ׂĂ̔ǂ�]��*/
double Solution::evaluation_all(const vector<Item>& items) {
    penalty = 0;
    relation = 0;
    ave_balance = 0;
    sum_balance = 0;
    auto [group_begin, group_end] = get_groups_range();
    for (auto citr = group_begin; citr != group_end; ++citr) {
        //�y�i���e�B�v�Z
        penalty += citr->calc_weight_penalty();
        penalty += citr->calc_sum_item_penalty(items);
        penalty += citr->calc_group_penalty(items);

        //value�̕������p
        for (size_t i = 0; i < Item::v_size; ++i) {
            ave_balance += std::abs(aves[i] - citr->value_average(i));
            //deviation += std::abs(sum_values[i] / Item::N - value_averages[i]);
            sum_balance += std::abs(sum_values[i] / Group::N - citr->get_sum_values()[i]);
        }

        //�֌W�l�̌v�Z
        for (const auto& r : citr->sum_item_relation(items)) {
            relation += r;
        }
        for (const auto& r : citr->sum_group_relation(items)) {
            relation += r;
        }
    }
    ave_balance /= Group::N;

    return get_eval_value();
}

auto Solution::evaluation_diff(const vector<MoveItem>& move_items) -> std::tuple<double, double, double, double> {
    vector<vector<const Item*>> in(Group::N + 1);
    vector<vector<const Item*>> out(Group::N + 1);
    for (const auto& mi : move_items) {
        out[mi.source].push_back(&mi.item);
        in[mi.destination].push_back(&mi.item);
    }

    //�y�i���e�B�̍������v�Z
    double diff_penalty = 0;
    for (size_t i = 0; i < Group::N; ++i) {
        diff_penalty += groups[i].diff_weight_penalty(in[i], out[i]);
        for (const auto& in_item : in[i]) {
            diff_penalty += in_item->group_penalty[i];
        }
        for (const auto& out_item : out[i]) {
            diff_penalty -= out_item->group_penalty[i];
        }
    }
    for (const auto& mi : move_items) {
        diff_penalty -= get_each_group_item_penalty(mi.item, mi.source);
        diff_penalty += get_each_group_item_penalty(mi.item, mi.destination);
        for (const auto& out_item : out[mi.destination]) {
            diff_penalty -= mi.item.item_penalty[out_item->id];
        }
    }
    for (size_t i = 0; i < Group::N; ++i) {
        for (auto itr1 = out[i].begin(), end = out[i].end(); itr1 != end; ++itr1) {
            for (auto itr2 = std::next(itr1); itr2 != end; ++itr2) {
                diff_penalty += (*itr1)->item_penalty[(*itr2)->id];
            }
        }
        for (auto itr1 = in[i].begin(), end = in[i].end(); itr1 != end; ++itr1) {
            for (auto itr2 = std::next(itr1); itr2 != end; ++itr2) {
                diff_penalty += (*itr1)->item_penalty[(*itr2)->id];
            }
        }
    }

    //�֌W�l�̍������v�Z
    double diff_relation = 0;
    for (size_t i = 0; i < Item::item_r_size; ++i) {
        for (const auto& mi : move_items) {
            diff_relation -= get_each_group_item_relation(mi.item, mi.source)[i];
            diff_relation += get_each_group_item_relation(mi.item, mi.destination)[i];
            for (const auto& out_item : out[mi.destination]) {
                diff_relation -= mi.item.item_relations[i][out_item->id];
            }
        }
        for (size_t j = 0; j < Group::N; ++j) {
            for (auto itr1 = out[j].begin(), end = out[j].end(); itr1 != end; ++itr1) {
                for (auto itr2 = std::next(itr1); itr2 != end; ++itr2) {
                    diff_relation += (*itr1)->item_relations[i][(*itr2)->id];
                }
            }
            for (auto itr1 = in[j].begin(), end = in[j].end(); itr1 != end; ++itr1) {
                for (auto itr2 = std::next(itr1); itr2 != end; ++itr2) {
                    diff_relation += (*itr1)->item_relations[i][(*itr2)->id];
                }
            }
        }
    }
    for (size_t i = 0; i < Item::group_r_size; ++i) {
        for (size_t j = 0; j < Group::N; ++j) {
            for (const auto& in_item : in[j]) {
                diff_relation += in_item->group_relations[i][j];
            }
            for (const auto& out_item : out[j]) {
                diff_relation -= out_item->group_relations[i][j];
            }
        }
    }
    
    //ave_balance��sum_balance�̍������v�Z
    double diff_ave_balance = 0, diff_sum_balance = 0;
    for (size_t i = 0; i < Group::N; ++i) {
        if (in[i].size() == 0 && out[i].size() == 0) continue;
        for (size_t j = 0; j < Item::v_size; ++j) {
            diff_ave_balance -= std::abs(groups[i].value_average(j) - aves[j]) / Group::N;
            diff_sum_balance -= std::abs(groups[i].get_sum_values()[j] - sum_values[j] / Group::N);

            double new_sum_score = groups[i].get_sum_values()[j];
            for (const auto& in_item : in[i]) {
                new_sum_score += in_item->values[j];
            }
            for (const auto& out_item : out[i]) {
                new_sum_score -= out_item->values[j];
            }
            double new_group_ave = new_sum_score / (groups[i].get_member_num() - out[i].size() + in[i].size());
            diff_ave_balance += std::abs(new_group_ave - aves[j]) / Group::N;
            diff_sum_balance += std::abs(new_sum_score - sum_values[j] / Group::N);
        }
    }

    //�v�Z�덷�̑΍�
    if (std::abs(diff_penalty) < 1e-10) diff_penalty = 0;
    if (std::abs(diff_relation) < 1e-10) diff_relation = 0;
    if (std::abs(diff_ave_balance) < 1e-10) diff_ave_balance = 0;
    if (std::abs(diff_sum_balance) < 1e-10) diff_sum_balance = 0;
    
    return {diff_penalty, diff_relation, diff_ave_balance, diff_sum_balance};
}

/*shift�ړ����̕]���l�̕ω���*/
auto Solution::evaluation_shift(const Item& item, int group_id) -> std::tuple<double, double, double, double> {
    const Group& now_group = groups[item_group_ids[item.id]];
    const Group& next_group = groups[group_id];

    //�y�i���e�B�̍������v�Z
    double diff_penalty = now_group.diff_weight_penalty({}, {&item}) + next_group.diff_weight_penalty({&item}, {});
    diff_penalty -= get_each_group_item_penalty(item, now_group.get_id());
    diff_penalty += get_each_group_item_penalty(item, next_group.get_id());
    diff_penalty -= item.group_penalty[now_group.get_id()];
    diff_penalty += item.group_penalty[next_group.get_id()];

    //�֌W�l�̍������v�Z
    double diff_relation = 0;
    for (size_t i = 0; i < Item::item_r_size; ++i) {
        diff_relation -= get_each_group_item_relation(item, now_group.get_id())[i];
        diff_relation += get_each_group_item_relation(item, next_group.get_id())[i];
    }
    for (size_t i = 0; i < Item::group_r_size; ++i) {
        diff_relation -= item.group_relations[i][now_group.get_id()];
        diff_relation += item.group_relations[i][next_group.get_id()];
    }

    //ave_balance��sum_balance�̍������v�Z
    double diff_ave_balance = 0, diff_sum_balance = 0;
    for (size_t i = 0; i < Item::v_size; ++i) {
        diff_ave_balance -= std::abs(now_group.value_average(i) - aves[i]) / Group::N;
        diff_ave_balance -= std::abs(next_group.value_average(i) - aves[i]) / Group::N;

        double group_ave = (double)(now_group.get_sum_values()[i] - item.values[i]) / (now_group.get_member_num() - 1);
        diff_ave_balance += std::abs(group_ave - aves[i]) / Group::N;
        group_ave = (double)(next_group.get_sum_values()[i] + item.values[i]) / (next_group.get_member_num() + 1);
        diff_ave_balance += std::abs(group_ave - aves[i]) / Group::N;

        diff_sum_balance -= std::abs(now_group.get_sum_values()[i] - sum_values[i] / Group::N);
        diff_sum_balance -= std::abs(next_group.get_sum_values()[i] - sum_values[i] / Group::N);
        diff_sum_balance += std::abs(now_group.get_sum_values()[i] - item.values[i] - sum_values[i] / Group::N);
        diff_sum_balance += std::abs(next_group.get_sum_values()[i] + item.values[i] - sum_values[i] / Group::N);
    }

    //�v�Z�덷�̑΍�
    if (std::abs(diff_penalty) < 1e-10) diff_penalty = 0;
    if (std::abs(diff_relation) < 1e-10) diff_relation = 0;
    if (std::abs(diff_ave_balance) < 1e-10) diff_ave_balance = 0;
    if (std::abs(diff_sum_balance) < 1e-10) diff_sum_balance = 0;
    
    return {diff_penalty, diff_relation, diff_ave_balance, diff_sum_balance};
}

/*swap�ړ����̕]���l�̕ω���*/
auto Solution::evaluation_swap(const Item& item1, const Item& item2) -> std::tuple<double, double, double, double> {
    const Group& g1 = groups[item_group_ids[item1.id]];
    const Group& g2 = groups[item_group_ids[item2.id]];

    //�y�i���e�B�̍������v�Z
    double diff_penalty = 0;
    diff_penalty += g1.diff_weight_penalty({&item1}, {&item2});
    diff_penalty += g2.diff_weight_penalty({&item2}, {&item1});
    
    diff_penalty += get_each_group_item_penalty(item1, g2.get_id());
    diff_penalty += get_each_group_item_penalty(item2, g1.get_id());
    diff_penalty -= get_each_group_item_penalty(item1, g1.get_id());
    diff_penalty -= get_each_group_item_penalty(item2, g2.get_id());
    diff_penalty -= item1.item_penalty[item2.id] * 2;

    diff_penalty += item1.group_penalty[g2.get_id()];
    diff_penalty += item2.group_penalty[g1.get_id()];
    diff_penalty -= item1.group_penalty[g1.get_id()];
    diff_penalty -= item2.group_penalty[g2.get_id()];

    //�֌W�l�̍������v�Z
    double diff_relation = 0;
    for (size_t i = 0; i < Item::item_r_size; ++i) {
        diff_relation += get_each_group_item_relation(item1, g2.get_id())[i];
        diff_relation += get_each_group_item_relation(item2, g1.get_id())[i];
        diff_relation -= get_each_group_item_relation(item1, g1.get_id())[i];
        diff_relation -= get_each_group_item_relation(item2, g2.get_id())[i];

        diff_relation -= item1.item_relations[i][item2.id] * 2;
    }
    for (size_t i = 0; i < Item::group_r_size; ++i) {
        diff_relation += item1.group_relations[i][g2.get_id()];
        diff_relation += item2.group_relations[i][g1.get_id()];
        diff_relation -= item1.group_relations[i][g1.get_id()];
        diff_relation -= item2.group_relations[i][g2.get_id()];
    }

    //ave_balance��sum_balance�̍������v�Z
    double diff_ave_balance = 0, diff_sum_balance = 0;
    for (size_t i = 0; i < Item::v_size; ++i) {
        diff_ave_balance -= std::abs(g1.value_average(i) - aves[i]) / Group::N;
        diff_ave_balance -= std::abs(g2.value_average(i) - aves[i]) / Group::N;

        double group_ave = (double)(g1.get_sum_values()[i] - item1.values[i] + item2.values[i]) / g1.get_member_num();
        diff_ave_balance += std::abs(group_ave - aves[i]) / Group::N;
        group_ave = (double)(g2.get_sum_values()[i] - item2.values[i] + item1.values[i]) / g2.get_member_num();
        diff_ave_balance += std::abs(group_ave - aves[i]) / Group::N;

        diff_sum_balance -= std::abs(g1.get_sum_values()[i] - sum_values[i] / Group::N);
        diff_sum_balance -= std::abs(g2.get_sum_values()[i] - sum_values[i] / Group::N);
        diff_sum_balance += std::abs(g1.get_sum_values()[i] - item1.values[i] + item2.values[i] - sum_values[i] / Group::N);
        diff_sum_balance += std::abs(g2.get_sum_values()[i] - item2.values[i] + item1.values[i] - sum_values[i] / Group::N);
    }

    //�v�Z�덷�̑΍�
    if (std::abs(diff_penalty) < 1e-10) diff_penalty = 0;
    if (std::abs(diff_relation) < 1e-10) diff_relation = 0;
    if (std::abs(diff_ave_balance) < 1e-10) diff_ave_balance = 0;
    if (std::abs(diff_sum_balance) < 1e-10) diff_sum_balance = 0;
    
    return {diff_penalty, diff_relation, diff_ave_balance, diff_sum_balance};
}

void Solution::move_processing(const std::vector<MoveItem>& move_items, const std::tuple<int, int, double, int>& diff) {
    auto [diff_relation, diff_penalty, diff_deviation, diff_balance] = diff;
    set_eval_value(relation + diff_relation, penalty + diff_penalty, deviation + diff_deviation, balance + diff_balance);
    for (const auto& mi : move_items) {
        for (int i = 0; i < Item::N; ++i) {
            if (each_group_item_relation[i][mi.source] && mi.source < Group::N) {
                each_group_item_relation[i][mi.source].value() -= (mi.item.relations[i] - mi.item.times[i]);
            }
            if (each_group_item_relation[i][mi.destination] && mi.destination < Group::N) {
                each_group_item_relation[i][mi.destination].value() += (mi.item.relations[i] - mi.item.times[i]);
            }
            if (group_score_distance[i][mi.source] && mi.source < Group::N) {
                group_score_distance[i][mi.source].value() -= mi.item.score_distances[i];
            }
            if (group_score_distance[i][mi.destination] && mi.destination < Group::N) {
                group_score_distance[i][mi.destination].value() += mi.item.score_distances[i];
            }
         }
    }
    for (const auto& mi : move_items) {
        //std::cerr << mi.item.id << " ";
        assert(item_group_ids[mi.item.id] == mi.source);
        groups[mi.source].erase_member(mi.item);
        groups[mi.destination].add_member(mi.item);
        item_group_ids[mi.item.id] = mi.destination;
    }
}

/*
 *shift�ړ����邩�ǂ����𒲍���, �K�v�ɉ����Ĉړ�����
 *�ړ������ꍇ��true, ���Ă��Ȃ��ꍇ��false��Ԃ�
 */
bool Solution::shift_check(Item& item, int group_id) {
    auto diff = evaluation_shift(item, group_id);
    auto [diff_relation, diff_penalty, diff_deviation, diff_balance] = diff;
    double increace = diff_relation * relation_parameter - diff_penalty * penalty_parameter - diff_deviation * score_parameter + diff_balance * balance_parameter;
    if (increace > 0 && std::abs(increace) > 1e-10) {
        vector<MoveItem> move_items = {MoveItem(item, item_group_ids[item.id], group_id)};
        move_processing(move_items, diff);
        //�l�̍X�V
        /*set_eval_value(relation + diff_relation, penalty + diff_penalty, deviation + diff_deviation);
        for (int i = 0; i < Item::N; ++i) {
            if (each_group_item_relation[i][item.group_id]) {
                each_group_item_relation[i][item.group_id].value() -= (item.relations[i] - item.times[i]);
            }
            if (each_group_item_relation[i][group_id]) {
                each_group_item_relation[i][group_id].value() += (item.relations[i] - item.times[i]);
            }
        }

        //�ړ�
        groups[item.group_id].erase_member(item);
        groups[group_id].add_member(item);*/
        
        return true;
    }
    return false;
}

/*
 *swap�ړ����邩�ǂ����𒲍���, �K�v�ɉ����Ĉړ�����
 *�ړ������ꍇ��true, ���Ă��Ȃ��ꍇ��false��Ԃ�
 */
bool Solution::swap_check(Item& item1, Item& item2) {
    auto diff = evaluation_swap(item1, item2);
    auto [diff_relation, diff_penalty, diff_deviation, diff_balance] = diff;
    double increace = diff_relation * relation_parameter - diff_penalty * penalty_parameter - diff_deviation * score_parameter + diff_balance * balance_parameter;
    if (increace > 0 && std::abs(increace) > 1e-10) {
        int g1_id = item_group_ids[item1.id], g2_id = item_group_ids[item2.id];
        vector<MoveItem> move_items = {MoveItem(item1, g1_id, g2_id), MoveItem(item2, g2_id, g1_id)};
        move_processing(move_items, diff);
        /*int group_id1 = item1.group_id;
        int group_id2 = item2.group_id;

        //�l�̍X�V
        set_eval_value(relation + diff_relation, penalty + diff_penalty, deviation + diff_deviation);
        for (int i = 0; i < Item::N; ++i) {
            //���[�_�[����(�X�}�[�g�ȕ��@�ɉ��P������)
            if (each_group_item_relation[i][group_id1]) {
                each_group_item_relation[i][group_id1].value() -= (item1.relations[i] - item1.times[i]);
                each_group_item_relation[i][group_id1].value() += (item2.relations[i] - item2.times[i]);
                each_group_item_relation[i][group_id2].value() -= (item2.relations[i] - item2.times[i]);
                each_group_item_relation[i][group_id2].value() += (item1.relations[i] - item1.times[i]);
            }
        }

        //�ړ�
        groups[group_id1].erase_member(item1);
        groups[group_id2].erase_member(item2);
        groups[group_id1].add_member(item2);
        groups[group_id2].add_member(item1);*/
        return true;
    }
    return false;
}

bool Solution::move_check(const vector<MoveItem>& move_items) {
    auto diff = evaluation_diff(move_items);
    auto [diff_relation, diff_penalty, diff_deviation, diff_balance] = diff;
    double increace = diff_relation * relation_parameter - diff_penalty * penalty_parameter - diff_deviation * score_parameter + diff_balance * balance_parameter;

    if (increace > 0) {
        move_processing(move_items, diff);
        return true;
    }
    return false;
}

void Solution::move(const vector<MoveItem>& move_items) {
    move_processing(move_items, evaluation_diff(move_items));
}

std::ostream& operator<<(std::ostream& out, const Solution& s) {
    auto [group_begin, group_end] = s.get_groups_range();
    for (auto citr = group_begin; citr != group_end; ++citr) {
        out << *citr << std::endl;
    }
    out << "�]���l:" << s.get_eval_value() << std::endl;
    return out;
}

/*void Solution::relation_greedy(vector<Item>& items) {
    for (auto&& item : items) {
        if (item.is_leader) {
            groups[item.group_id].add_member(item);
        }
    }
    vector<int> shuffle_ids(items.size());
    std::iota(shuffle_ids.begin(), shuffle_ids.end(), 0);
    MyRandom::shuffle(shuffle_ids);
    for (const auto& id : shuffle_ids) {
        if (!items[id].is_leader) {
            int max_group_id;
            double max_value = -DBL_MAX;
            auto [group_begin, group_end] = get_groups_range();
            for (auto citr = group_begin; citr != group_end; ++citr) {
                double value;
                if ((value = citr->each_group_item_relation(items[id]) / (double)citr->get_member_num()) > max_value) {
                    max_value = value;
                    max_group_id = citr->get_id();
                }
            }
            groups[max_group_id].add_member(items[id]);
        }
    }
}

void Solution::penalty_greedy(vector<Item>& items) {
    for (auto&& item : items) {
        if (item.is_leader) {
            groups[item.group_id].add_member(item);
        }
    }
    vector<int> shuffle_ids(items.size());
    std::iota(shuffle_ids.begin(), shuffle_ids.end(), 0);
    MyRandom::shuffle(shuffle_ids);
    for (const auto& id : shuffle_ids) {
        Item& item = items[id];
        if (!item.is_leader) {
            int min_group_id;
            int min_penalty = INT_MAX;
            auto [group_begin, group_end] = get_groups_range();
            for (auto citr = group_begin; citr != group_end; ++citr) {
                int diff_penalty = citr->diff_penalty({&item}, {});
                if (diff_penalty < min_penalty) {
                    min_group_id = citr->get_id();
                    min_penalty = diff_penalty;
                }
            }
            //std::cerr << min_penalty << std::endl;
            groups[min_group_id].add_member(item);
        }
    }
}

void Solution::score_greedy(vector<Item>& items) {
    std::sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return a.score > b.score;
    });
    vector<vector<int>> blocks(Item::N / Group::N + 1);
    for (int i = 0; i < Item::N; ++i) {
        blocks[i / Group::N].push_back(items[i].id);
    }
    std::sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return a.id < b.id;
    });
    for (int i = 0; i < blocks.size(); ++i) {
        if (i % 4 == 1 || i % 4 == 2) {
            std::reverse(blocks[i].begin(), blocks[i].end());
        }
        for (int j = 0; j < blocks[i].size(); ++j) {
            if (items[blocks[i][j]].is_leader && items[blocks[i][j]].group_id != j) {
                std::swap(blocks[i][j], blocks[i][items[blocks[i][j]].group_id]);
            }
        }
    }

    for (auto&& block : blocks) {
        for (int i = 0; i < block.size(); ++i) {
            groups[i].add_member(items[block[i]]);
        }
    }
}*/