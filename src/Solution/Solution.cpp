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
    for (size_t i = 0; i < Group::N + 1; i++) {
        groups.push_back(Group(i));
    }
    group_relation.assign(Item::N, vector<std::optional<vector<double>>>(Group::N + 1, std::nullopt));
    item_group_ids.resize(Item::N);
    aves.assign(Item::v_size, 0);
    sum_values.assign(Item::v_size, 0);

    //班長は指定の班, それ以外はランダムな班に追加
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
        for (size_t i = 0; i < Item::v_size; i++) {
            sum_values[i] += item.values[i];
        }
    }

    for (size_t i = 0; i < Item::v_size; i++) {
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

const vector<double>& Solution::get_group_relation(const Item& item, int group_id) {
    if (group_id >= Group::N) {
        group_relation[item.id][group_id] = vector<double>(item.item_relations.size(), 0);
    }
    else if (!group_relation[item.id][group_id]) {
        group_relation[item.id][group_id] = groups[group_id].item_relation(item);
    }
    return group_relation[item.id][group_id].value();
}

/*すべての班を評価*/
double Solution::evaluation_all(const vector<Item>& items) {
    penalty = 0;
    relation = 0;
    ave_balance = 0;
    sum_balance = 0;
    auto [group_begin, group_end] = get_groups_range();
    for (auto citr = group_begin; citr != group_end; ++citr) {
        penalty += citr->calc_weight_penalty();
        penalty += citr->calc_sum_item_penalty(items);
        penalty += citr->calc_group_penalty(items);

        //点数の平滑化用
        vector<double> value_averages = citr->value_averages();
        vector<double> group_sum_values = citr->get_sum_values();
        for (size_t i = 0; i < Item::v_size; i++) {
            ave_balance += std::abs(aves[i] - value_averages[i]);
            //deviation += std::abs(sum_values[i] / Item::N - value_averages[i]);
            sum_balance += std::abs(sum_values[i] - group_sum_values[i]);
        }

        //関係値と回数の和
        for (const auto& sum_relation : citr->sum_relation(items)) {
            relation += sum_relation;
        }
    }
    ave_balance /= Group::N;

    return get_eval_value();
}

auto Solution::evaluation_diff(const vector<MoveItem>& move_items) -> std::tuple<int, int, double, int> {
    vector<vector<const Item*>> in(Group::N + 1);
    vector<vector<const Item*>> out(Group::N + 1);
    for (const auto& mi : move_items) {
        out[mi.source].push_back(&mi.item);
        in[mi.destination].push_back(&mi.item);
    }

    int diff_penalty = 0;
    for (int i = 0; i < Group::N; ++i) {
        diff_penalty += groups[i].diff_penalty(in[i], out[i]);
    }

    int decreace = 0, increace = 0, sd = 0;
    for (const auto& mi : move_items) {
        decreace += get_group_relation(mi.item, mi.source);
        increace += get_group_relation(mi.item, mi.destination);
        sd -= get_group_score_distance(mi.item, mi.source);
        sd += get_group_score_distance(mi.item, mi.destination);
        for (const auto& ptr_p : out[mi.destination]) {
            decreace += mi.item.relations[ptr_p->id] - mi.item.times[ptr_p->id];
            sd -= mi.item.score_distances[ptr_p->id];
        }
    }
    for (int i = 0; i < Group::N; ++i) {
        for (auto itr1 = out[i].begin(), end = out[i].end(); itr1 != end; ++itr1) {
            for (auto itr2 = std::next(itr1); itr2 != end; ++itr2) {
                increace += (*itr1)->relations[(*itr2)->id] - (*itr1)->times[(*itr2)->id];
                sd += (*itr1)->score_distances[(*itr2)->id];
            }
        }
        for (auto itr1 = in[i].begin(), end = in[i].end(); itr1 != end; ++itr1) {
            for (auto itr2 = std::next(itr1); itr2 != end; ++itr2) {
                increace += (*itr1)->relations[(*itr2)->id] - (*itr1)->times[(*itr2)->id];
                sd += (*itr1)->score_distances[(*itr2)->id];
            }
        }
    }

    double diff_deviation = 0;
    //double next_dispersion = std::pow(deviation, 2);
    for (int i = 0; i < Group::N; ++i) {
        if (in[i].size() == 0 && out[i].size() == 0) continue;

        diff_deviation -= std::abs(groups[i].score_average() - ave) / Group::N;
        //next_dispersion -= std::pow(groups[i].score_average() - ave, 2);
        int new_sum_score = groups[i].get_sum_score();
        for (const auto& ptr_p : in[i]) {
            new_sum_score += ptr_p->score;
        }
        for (const auto& ptr_p : out[i]) {
            new_sum_score -= ptr_p->score;
        }
        double new_group_ave = (double)new_sum_score / (groups[i].get_member_num() - out[i].size() + in[i].size());
        diff_deviation += std::abs(new_group_ave - ave) / Group::N;
        //next_dispersion += std::pow(new_group_ave - ave, 2);
    }

    //double next_deviation = std::sqrt(next_dispersion);

    //計算誤差の対策
    if (std::abs(diff_deviation) < 1e-10 && increace == decreace && diff_penalty == 0 && sd == 0) {
        return {0, 0, 0, 0};
    }
    //std::cerr << increace - decreace << " " << diff_penalty << " " << next_deviation - std::sqrt(dispersion) << std::endl;

    //return increace - decreace - diff_penalty * penalty_parameter - (next_deviation - std::sqrt(dispersion)) * score_parameter;
    return {increace - decreace, diff_penalty, diff_deviation, sd};
}

/*shift移動時の評価値の変化量*/
auto Solution::evaluation_shift(const Item& item, int group_id) -> std::tuple<int, int, double, int> {
    //関係値と回数の和の変化量を計算
    const Group& now_group = groups[item_group_ids[item.id]];
    int decreace, increace;
    decreace = get_group_relation(item, now_group.get_id());

    const Group& next_group = groups[group_id];
    increace = get_group_relation(item, next_group.get_id());

    int sd = get_group_score_distance(item, next_group.get_id()) - get_group_score_distance(item, now_group.get_id());

    //ペナルティの変化量を計算
    /*int diff_penalty = now_group.erase_penalty(upper_num[item.year - 1][item.gender][item.campus], 
                                          lower_num[item.year - 1][item.gender][item.campus], item);
    
    diff_penalty += next_group.add_penalty(upper_num[item.year - 1][item.gender][item.campus],
                                      lower_num[item.year - 1][item.gender][item.campus], item);*/
    int diff_penalty = now_group.diff_penalty({}, {&item});
    diff_penalty += next_group.diff_penalty({&item}, {});
    
    //点数の平均偏差の算出
    double diff_deviation = 0;
    diff_deviation -= std::abs(now_group.score_average() - ave) / Group::N;
    diff_deviation -= std::abs(next_group.score_average() - ave) / Group::N;
    //double next_dispersion = std::pow(deviation, 2);
    //next_dispersion -= std::pow(now_group.score_average() - ave, 2);
    //next_dispersion -= std::pow(next_group.score_average() - ave, 2);

    double group_ave = (double)(now_group.get_sum_score() - item.score) / (now_group.get_member_num() - 1);
    diff_deviation += std::abs(group_ave - ave) / Group::N;
    //next_dispersion += std::pow(group_ave - ave, 2);
    group_ave = (double)(next_group.get_sum_score() + item.score) / (next_group.get_member_num() + 1);
    diff_deviation += std::abs(group_ave - ave) / Group::N;
    //next_dispersion += std::pow(group_ave - ave, 2);

    //double next_deviation = std::sqrt(next_dispersion);

    //計算誤差の対策
    if (std::abs(diff_deviation) < 1e-10 && increace == decreace && diff_penalty == 0, sd == 0) {
        return {0, 0, 0, 0};
    }

    return {increace - decreace, diff_penalty, diff_deviation, sd};
}

/*swap移動時の評価値の変化量*/
auto Solution::evaluation_swap(const Item& item1, const Item& item2) -> std::tuple<int, int, double, int> {
    //関係値と回数の和の変化量を計算(shift移動時の計算結果を利用)
    const Group& g1 = groups[item_group_ids[item1.id]];
    const Group& g2 = groups[item_group_ids[item2.id]];
    int diff_relation = 0, sd = 0;
    diff_relation += get_group_relation(item1, g2.get_id());
    diff_relation += get_group_relation(item2, g1.get_id());
    diff_relation -= get_group_relation(item1, g1.get_id());
    diff_relation -= get_group_relation(item2, g2.get_id());
    sd += get_group_score_distance(item1, g2.get_id());
    sd += get_group_score_distance(item2, g1.get_id());
    sd -= get_group_score_distance(item1, g1.get_id());
    sd -= get_group_score_distance(item2, g2.get_id());

    diff_relation -= (item1.relations[item2.id] - item1.times[item2.id]) * 2;
    sd -= item1.score_distances[item2.id] * 2;

    //ペナルティの変化量を計算
    int diff_penalty = 0;
    if (item1.year != item2.year || item1.gender != item2.gender || item1.campus != item2.campus) {
        /*diff_penalty += groups[item1.group_id].erase_penalty(upper_num[item1.year - 1][item1.gender][item1.campus],
                                                     lower_num[item1.year - 1][item1.gender][item1.campus], item1);
        
        diff_penalty += groups[item1.group_id].add_penalty(upper_num[item2.year - 1][item2.gender][item2.campus],
                                                   lower_num[item2.year - 1][item2.gender][item2.campus], item2);

        diff_penalty += groups[item2.group_id].erase_penalty(upper_num[item2.year - 1][item2.gender][item2.campus],
                                                     lower_num[item2.year - 1][item2.gender][item2.campus], item2);
        
        diff_penalty += groups[item2.group_id].add_penalty(upper_num[item1.year - 1][item1.gender][item1.campus],
                                                   lower_num[item1.year - 1][item1.gender][item1.campus], item1);


        diff_penalty -= 4;*/
        diff_penalty += g1.diff_penalty({&item2}, {&item1});
        diff_penalty += g2.diff_penalty({&item1}, {&item2});
    }

    //点数の平均偏差を算出
    double diff_deviation = 0;
    //double next_dispersion = std::pow(deviation, 2);
    diff_deviation -= std::abs(g1.score_average() - ave) / Group::N;
    diff_deviation -= std::abs(g2.score_average() - ave) / Group::N;
    //next_dispersion -= std::pow(g1.score_average() - ave, 2);
    //next_dispersion -= std::pow(g2.score_average() - ave, 2);

    double group_ave = (double)(g1.get_sum_score() - item1.score + item2.score) / g1.get_member_num();
    diff_deviation += std::abs(group_ave - ave) / Group::N;
    //next_dispersion += std::pow(group_ave - ave, 2);
    group_ave = (double)(g2.get_sum_score() - item2.score + item1.score) / g2.get_member_num();
    diff_deviation += std::abs(group_ave - ave) / Group::N;
    //next_dispersion += std::pow(group_ave - ave, 2);

    //double next_deviation = std::sqrt(next_dispersion);

    //計算誤差の対策
    if (std::abs(diff_deviation) < 1e-10 && diff_relation == 0 && diff_penalty == 0, sd == 0) {
        return {0, 0, 0, 0};
    }

    return {diff_relation, diff_penalty, diff_deviation, sd};
}

void Solution::move_processing(const std::vector<MoveItem>& move_items, const std::tuple<int, int, double, int>& diff) {
    auto [diff_relation, diff_penalty, diff_deviation, diff_balance] = diff;
    set_eval_value(relation + diff_relation, penalty + diff_penalty, deviation + diff_deviation, balance + diff_balance);
    for (const auto& mi : move_items) {
        for (int i = 0; i < Item::N; ++i) {
            if (group_relation[i][mi.source] && mi.source < Group::N) {
                group_relation[i][mi.source].value() -= (mi.item.relations[i] - mi.item.times[i]);
            }
            if (group_relation[i][mi.destination] && mi.destination < Group::N) {
                group_relation[i][mi.destination].value() += (mi.item.relations[i] - mi.item.times[i]);
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
 *shift移動するかどうかを調査し, 必要に応じて移動する
 *移動した場合はtrue, していない場合はfalseを返す
 */
bool Solution::shift_check(Item& item, int group_id) {
    auto diff = evaluation_shift(item, group_id);
    auto [diff_relation, diff_penalty, diff_deviation, diff_balance] = diff;
    double increace = diff_relation * relation_parameter - diff_penalty * penalty_parameter - diff_deviation * score_parameter + diff_balance * balance_parameter;
    if (increace > 0 && std::abs(increace) > 1e-10) {
        vector<MoveItem> move_items = {MoveItem(item, item_group_ids[item.id], group_id)};
        move_processing(move_items, diff);
        //値の更新
        /*set_eval_value(relation + diff_relation, penalty + diff_penalty, deviation + diff_deviation);
        for (int i = 0; i < Item::N; i++) {
            if (group_relation[i][item.group_id]) {
                group_relation[i][item.group_id].value() -= (item.relations[i] - item.times[i]);
            }
            if (group_relation[i][group_id]) {
                group_relation[i][group_id].value() += (item.relations[i] - item.times[i]);
            }
        }

        //移動
        groups[item.group_id].erase_member(item);
        groups[group_id].add_member(item);*/
        
        return true;
    }
    return false;
}

/*
 *swap移動するかどうかを調査し, 必要に応じて移動する
 *移動した場合はtrue, していない場合はfalseを返す
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

        //値の更新
        set_eval_value(relation + diff_relation, penalty + diff_penalty, deviation + diff_deviation);
        for (int i = 0; i < Item::N; i++) {
            //リーダー避け(スマートな方法に改善したい)
            if (group_relation[i][group_id1]) {
                group_relation[i][group_id1].value() -= (item1.relations[i] - item1.times[i]);
                group_relation[i][group_id1].value() += (item2.relations[i] - item2.times[i]);
                group_relation[i][group_id2].value() -= (item2.relations[i] - item2.times[i]);
                group_relation[i][group_id2].value() += (item1.relations[i] - item1.times[i]);
            }
        }

        //移動
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
    out << "評価値:" << s.get_eval_value() << std::endl;
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
                if ((value = citr->group_relation(items[id]) / (double)citr->get_member_num()) > max_value) {
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
    for (int i = 0; i < Item::N; i++) {
        blocks[i / Group::N].push_back(items[i].id);
    }
    std::sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return a.id < b.id;
    });
    for (int i = 0; i < blocks.size(); i++) {
        if (i % 4 == 1 || i % 4 == 2) {
            std::reverse(blocks[i].begin(), blocks[i].end());
        }
        for (int j = 0; j < blocks[i].size(); j++) {
            if (items[blocks[i][j]].is_leader && items[blocks[i][j]].group_id != j) {
                std::swap(blocks[i][j], blocks[i][items[blocks[i][j]].group_id]);
            }
        }
    }

    for (auto&& block : blocks) {
        for (int i = 0; i < block.size(); i++) {
            groups[i].add_member(items[block[i]]);
        }
    }
}*/