#include "Solution.hpp"
#include <Item.hpp>
#include <Group.hpp>
#include <MyRandom.hpp>
#include <Search.hpp>
#include <Destroy.hpp>
#include <Input.hpp>

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
#include <algorithm>

using std::vector;

/*コンストラクタ*/
Solution::Solution(const Input& input) {
    //パラメータ設定
    penalty_param = 100;
    opt = input.get_opt();
    item_relation_params = input.get_item_relation_params();
    group_relation_params = input.get_group_relation_params();
    value_ave_params = input.get_value_ave_params();
    value_sum_params = input.get_value_sum_params();
    group_num_param = input.get_group_num_param();
    constant = input.get_constant();

    //最小化の場合はパラメータの符号を反転
    if (opt == Input::Opt::MIN) {
        std::for_each(item_relation_params.begin(), item_relation_params.end(), [](auto& p) { p *= -1; });
        std::for_each(group_relation_params.begin(), group_relation_params.end(), [](auto& p) { p *= -1; });
        std::for_each(value_ave_params.begin(), value_ave_params.end(), [](auto& p) { p *= -1; });
        std::for_each(value_sum_params.begin(), value_sum_params.end(), [](auto& p) { p *= -1; });
        penalty_param *= -1;
        group_num_param *= -1;
        constant *= -1;
    }

    //eval_flagsの設定
    eval_flags.set();
    if (Item::w_size == 0) eval_flags.reset(EvalIdx::WEIGHT_PENA);
    if (input.get_item_penalty_num() == 0) eval_flags.reset(EvalIdx::ITEM_PENA);
    if (input.get_group_penalty_num() == 0) eval_flags.reset(EvalIdx::GROUP_PENA);
    if (item_relation_params.size() == 0) eval_flags.reset(EvalIdx::ITEM_R);
    if (group_relation_params.size() == 0) eval_flags.reset(EvalIdx::GROUP_R);
    if (value_ave_params.size() == 0) eval_flags.reset(EvalIdx::VALUE_AVE);
    if (value_sum_params.size() == 0) eval_flags.reset(EvalIdx::VALUE_SUM);
    if (group_num_param == 0) eval_flags.reset(EvalIdx::GROUP_NUM);
    std::cout << eval_flags << std::endl;

    groups.clear();
    valid_groups.clear();
    groups.reserve(Group::N + 1);
    for (size_t i = 0; i < Group::N; ++i) {
        groups.push_back(Group(i, input.get_weight_upper(), input.get_weight_lower()));
        valid_groups.emplace_back(std::make_unique<const Group>(groups[i]));
    }
    groups.push_back(Group(Group::N));
    each_group_item_relation.assign(Item::N, vector<std::optional<vector<double>>>(Group::N + 1, std::nullopt));
    each_group_item_penalty.assign(Item::N, vector<std::optional<int>>(Group::N + 1, std::nullopt));
    item_group_ids.resize(Item::N);
    aves.assign(Item::v_size, 0);
    sum_values.assign(Item::v_size, 0);

    //指定グループがあるアイテムを指定グループに
    //それ以外のアイテムはダミーグループに
    int cnt = 0;
    for (auto&& item : input.get_items()) {
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
    RelationGreedy rg(input.get_items(), 1, 1);
    //PenaltyGreedy rg(items, 1);
    std::shared_ptr<Destroy> des = std::make_shared<Destroy>(input.get_items(), 1, 1);
    *this = *rg(*this, des);
}

Solution::Solution(const Solution& s) : groups(s.groups), item_group_ids(s.item_group_ids), relation(s.relation), penalty(s.penalty), ave_balance(s.ave_balance),
                                        sum_balance(s.sum_balance), each_group_item_relation(s.each_group_item_relation), each_group_item_penalty(s.each_group_item_penalty),
                                        aves(s.aves), sum_values(s.sum_values), opt(s.opt), item_relation_params(s.item_relation_params), group_relation_params(s.group_relation_params),
                                        value_ave_params(s.value_ave_params), value_sum_params(s.value_sum_params), penalty_param(s.penalty_param), group_num_param(s.group_num_param),
                                        constant(s.constant), eval_flags(s.eval_flags) {

    //groups = s.groups;
    for (size_t i = 0; i < Group::N; ++i) {
        if (groups[i].get_member_num() != 0) {
            valid_groups.emplace_back(std::make_unique<const Group>(groups[i]));
        }
    }
    //std::cerr << "コピーコンストラクタ" << std::endl;
}

/*each_group_item_relationの値を取得, なければ計算して取得*/
const vector<double>& Solution::get_each_group_item_relation(const Item& item, int group_id) {
    if (group_id >= Group::N) {
        each_group_item_relation[item.id][group_id] = vector<double>(Item::item_r_size, 0);
    }
    else if (!each_group_item_relation[item.id][group_id]) {
        each_group_item_relation[item.id][group_id] = groups[group_id].item_relation(item, item_relation_params);
    }
    return each_group_item_relation[item.id][group_id].value();
}

/*each_group_item_penaltyの値を取得, なければ計算して取得*/
int Solution::get_each_group_item_penalty(const Item& item, int group_id) {
    if (group_id >= Group::N) {
        each_group_item_penalty[item.id][group_id] = 0;
    }
    else if (!each_group_item_penalty[item.id][group_id]) {
        each_group_item_penalty[item.id][group_id] = groups[group_id].calc_item_penalty(item);
    }
    return each_group_item_penalty[item.id][group_id].value();
}

/*現在の解(グループ分け)を評価*/
double Solution::evaluation_all(const vector<Item>& items) {
    penalty = 0;
    relation = 0;
    ave_balance = 0;
    sum_balance = 0;
    auto [group_begin, group_end] = get_groups_range();
    for (auto g_itr = group_begin; g_itr != group_end; ++g_itr) {
        //ペナルティ計算
        if (eval_flags.test(EvalIdx::WEIGHT_PENA)) penalty += g_itr->calc_weight_penalty();
        if (eval_flags.test(EvalIdx::ITEM_PENA)) penalty += g_itr->calc_sum_item_penalty(items);
        if (eval_flags.test(EvalIdx::GROUP_PENA)) penalty += g_itr->calc_group_penalty(items);

        //関係値の計算
        if (eval_flags.test(EvalIdx::ITEM_R)) {
            for (const auto& r : g_itr->sum_item_relation(items, item_relation_params)) {
                relation += r;
            }
        }
        if (eval_flags.test(EvalIdx::GROUP_R)) {
            for (const auto& r : g_itr->sum_group_relation(items, group_relation_params)) {
                relation += r;
            }
        }

        //valueの平滑化用
        if (eval_flags.test(EvalIdx::VALUE_AVE)) {
            for (size_t i = 0; i < Item::v_size; ++i) {
                ave_balance += std::abs(aves[i] - g_itr->value_average(i)) * value_ave_params[i];
            }
        }
        if (eval_flags.test(EvalIdx::VALUE_SUM)) {
            for (size_t i = 0; i < Item::v_size; ++i) {
                if (eval_flags.test(EvalIdx::GROUP_NUM)) {
                    sum_balance += std::abs(sum_values[i] / valid_groups.size() - g_itr->get_sum_values()[i]) * value_sum_params[i];
                }
                else {
                    sum_balance += std::abs(sum_values[i] / Group::N - g_itr->get_sum_values()[i]) * value_sum_params[i];
                }
            }
        }
    }
    ave_balance /= Group::N;

    return get_eval_value();
}

/*評価値の変化量を計算*/
auto Solution::evaluation_diff(const vector<MoveItem>& move_items) -> std::tuple<double, double, double, double, int> {
    //std::cerr << "eval_diff" << std::endl;
    vector<vector<const Item*>> in(Group::N);
    vector<vector<const Item*>> out(Group::N);
    for (const auto& mi : move_items) {
        if (mi.source < Group::N) out[mi.source].push_back(&mi.item);
        if (mi.destination < Group::N) in[mi.destination].push_back(&mi.item);
    }
    //std::cerr << "eval_diff" << std::endl;
    //グループ数の増減を計算
    int diff_group_num = 0;
    if (eval_flags.test(EvalIdx::GROUP_NUM)) {
        for (size_t i = 0; i < Group::N; ++i) {
            int member_num = groups[i].get_member_num();
            if (member_num == 0) {
                if (in[i].size() != 0) ++diff_group_num;
            }
            else {
                if (member_num - out[i].size() + in[i].size() == 0) --diff_group_num;
            }
        }
    }
    //std::cerr << "eval_diff" << std::endl;
    //ペナルティの差分を計算
    double diff_penalty = 0;
    if (eval_flags.test(EvalIdx::WEIGHT_PENA)) {
        for (size_t i = 0; i < Group::N; ++i) {
            diff_penalty += groups[i].diff_weight_penalty(in[i], out[i]);
        }
    }
    //std::cerr << "eval_diff" << std::endl;
    if (eval_flags.test(EvalIdx::GROUP_PENA)) {
        for (size_t i = 0; i < Group::N; ++i) {
            for (const auto& in_item : in[i]) {
                diff_penalty += in_item->group_penalty[i];
            }
            for (const auto& out_item : out[i]) {
                diff_penalty -= out_item->group_penalty[i];
            }
        }
    }
    //std::cerr << "eval_diff" << std::endl;
    if (eval_flags.test(EvalIdx::ITEM_PENA)) {
        //std::cerr << "eval_diff" << std::endl;
        for (const auto& mi : move_items) {
            diff_penalty -= get_each_group_item_penalty(mi.item, mi.source);
            diff_penalty += get_each_group_item_penalty(mi.item, mi.destination);
            if (mi.destination == Group::N) continue;
            for (const auto& out_item : out[mi.destination]) {
                diff_penalty -= mi.item.item_penalty[out_item->id];
            }
        }
        //std::cerr << "eval_diff" << std::endl;
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
    }
    //std::cerr << "eval_diff" << std::endl;
    //関係値の差分を計算
    double diff_relation = 0;
    if (eval_flags.test(EvalIdx::ITEM_R)) {
        for (size_t i = 0; i < Item::item_r_size; ++i) {
            for (const auto& mi : move_items) {
                diff_relation -= get_each_group_item_relation(mi.item, mi.source)[i];
                diff_relation += get_each_group_item_relation(mi.item, mi.destination)[i];
                if (mi.destination == Group::N) continue;
                for (const auto& out_item : out[mi.destination]) {
                    diff_relation -= mi.item.item_relations[out_item->id][i] * item_relation_params[i];
                }
            }
            for (size_t j = 0; j < Group::N; ++j) {
                for (auto itr1 = out[j].begin(), end = out[j].end(); itr1 != end; ++itr1) {
                    for (auto itr2 = std::next(itr1); itr2 != end; ++itr2) {
                        diff_relation += (*itr1)->item_relations[(*itr2)->id][i] * item_relation_params[i];
                    }
                }
                for (auto itr1 = in[j].begin(), end = in[j].end(); itr1 != end; ++itr1) {
                    for (auto itr2 = std::next(itr1); itr2 != end; ++itr2) {
                        diff_relation += (*itr1)->item_relations[(*itr2)->id][i] * item_relation_params[i];
                    }
                }
            }
        }
    }
    //std::cerr << "eval_diff" << std::endl;
    if (eval_flags.test(EvalIdx::GROUP_R)) {
        for (size_t i = 0; i < Item::group_r_size; ++i) {
            for (size_t j = 0; j < Group::N; ++j) {
                for (const auto& in_item : in[j]) {
                    diff_relation += in_item->group_relations[j][i] * group_relation_params[i];
                }
                for (const auto& out_item : out[j]) {
                    diff_relation -= out_item->group_relations[j][i] * group_relation_params[i];
                }
            }
        }
    }
    //std::cerr << "eval_diff" << std::endl;
    //ave_balanceとsum_balanceの差分を計算
    double diff_ave_balance = 0, diff_sum_balance = 0;
    std::bitset<8> mask = (1<<EvalIdx::VALUE_AVE) | (1<<EvalIdx::VALUE_SUM);
    std::bitset<8> mask2 = (1<<EvalIdx::VALUE_SUM) | (1<<EvalIdx::GROUP_NUM);
    //std::cerr << "eval_diff" << std::endl;
    if ((eval_flags & mask2) == mask2) {
        diff_sum_balance = -sum_balance;
    }
    //std::cerr << "eval_diff" << std::endl;
    if ((eval_flags & mask).any()) {
        for (size_t i = 0; i < Group::N; ++i) {
            if (in[i].size() == 0 && out[i].size() == 0) {
                if ((eval_flags & mask2) == mask2) {
                    for (size_t j = 0; j < Item::v_size; ++j) {
                        diff_sum_balance += std::abs(groups[i].get_sum_values()[j] - sum_values[j] / (valid_groups.size() + diff_group_num)) * value_sum_params[j];
                    }
                }
                continue;
            }
            for (size_t j = 0; j < Item::v_size; ++j) {
                double new_sum_score = groups[i].get_sum_values()[j];
                for (const auto& in_item : in[i]) {
                    new_sum_score += in_item->values[j];
                }
                for (const auto& out_item : out[i]) {
                    new_sum_score -= out_item->values[j];
                }
                
                if (eval_flags.test(EvalIdx::VALUE_AVE)) {
                    double new_group_ave = new_sum_score / (groups[i].get_member_num() - out[i].size() + in[i].size());
                    diff_ave_balance -= (std::abs(groups[i].value_average(j) - aves[j]) / Group::N) * value_ave_params[j];
                    diff_ave_balance += (std::abs(new_group_ave - aves[j]) / Group::N) * value_ave_params[j];
                }
                if (eval_flags.test(EvalIdx::VALUE_SUM)) {
                    if (eval_flags.test(EvalIdx::GROUP_NUM)) {
                        diff_sum_balance += std::abs(new_sum_score - sum_values[j] / (valid_groups.size() + diff_group_num)) * value_sum_params[j];
                    }
                    else {
                        diff_sum_balance -= std::abs(groups[i].get_sum_values()[j] - sum_values[j] / Group::N) * value_sum_params[j];
                        diff_sum_balance += std::abs(new_sum_score - sum_values[j] / Group::N) * value_sum_params[j];
                    }
                }
            }
        }
    }
    //std::cerr << "eval_diff" << std::endl;
    //計算誤差の対策
    if (std::abs(diff_penalty) < 1e-10) diff_penalty = 0;
    if (std::abs(diff_relation) < 1e-10) diff_relation = 0;
    if (std::abs(diff_ave_balance) < 1e-10) diff_ave_balance = 0;
    if (std::abs(diff_sum_balance) < 1e-10) diff_sum_balance = 0;
    
    return {diff_penalty, diff_relation, diff_ave_balance, diff_sum_balance, diff_group_num};
}

/*shift移動時の評価値の変化量を計算*/
auto Solution::evaluation_shift(const Item& item, int group_id) -> std::tuple<double, double, double, double, int> {
    //std::cerr << "eval_shi" << std::endl;
    const Group& now_group = groups[item_group_ids[item.id]];
    const Group& next_group = groups[group_id];
    //std::cerr << "g_num" << std::endl;
    //グループ数の増減を計算
    int diff_group_num = 0;
    if (eval_flags.test(EvalIdx::GROUP_NUM)) {
        if (now_group.get_member_num() == 1) --diff_group_num;
        if (next_group.get_member_num() == 0) ++diff_group_num;
    }

    //ペナルティの差分を計算
    double diff_penalty = 0;
    //std::cerr << "w_pena" << std::endl;
    if (eval_flags.test(EvalIdx::WEIGHT_PENA)) {
        diff_penalty += now_group.diff_weight_penalty({}, {&item}) + next_group.diff_weight_penalty({&item}, {});
        //std::cerr << item.id << " " << group_id << " " << diff_penalty << std::endl;
    }
    //std::cerr << "i_pena" << std::endl;
    if (eval_flags.test(EvalIdx::ITEM_PENA)) {
        diff_penalty -= get_each_group_item_penalty(item, now_group.get_id());
        diff_penalty += get_each_group_item_penalty(item, next_group.get_id());
    }
    //std::cerr << "g_pena" << std::endl;
    if (eval_flags.test(EvalIdx::GROUP_PENA)) {
        diff_penalty -= item.group_penalty[now_group.get_id()];
        diff_penalty += item.group_penalty[next_group.get_id()];
    }

    //関係値の差分を計算
    double diff_relation = 0;
    //std::cerr << "i_r" << std::endl;
    if (eval_flags.test(EvalIdx::ITEM_R)) {
        for (size_t i = 0; i < Item::item_r_size; ++i) {
            diff_relation -= get_each_group_item_relation(item, now_group.get_id())[i];
            diff_relation += get_each_group_item_relation(item, next_group.get_id())[i];
        }
    }
    //std::cerr << "g_r" << std::endl;
    if (eval_flags.test(EvalIdx::GROUP_R)) {
        for (size_t i = 0; i < Item::group_r_size; ++i) {
            diff_relation -= item.group_relations[now_group.get_id()][i] * group_relation_params[i];
            diff_relation += item.group_relations[next_group.get_id()][i] * group_relation_params[i];
        }
    }

    //ave_balanceとsum_balanceの差分を計算
    double diff_ave_balance = 0, diff_sum_balance = 0;
    std::bitset<8> mask = (1<<EvalIdx::VALUE_AVE) | (1<<EvalIdx::VALUE_SUM);
    //std::cerr << "v_ave" << std::endl;
    if (eval_flags.test(EvalIdx::VALUE_AVE)) {
        for (size_t i = 0; i < Item::v_size; ++i) {
            diff_ave_balance -= (std::abs(now_group.value_average(i) - aves[i]) / Group::N) * value_ave_params[i];
            diff_ave_balance -= (std::abs(next_group.value_average(i) - aves[i]) / Group::N) * value_ave_params[i];

            double group_ave = (double)(now_group.get_sum_values()[i] - item.values[i]) / (now_group.get_member_num() - 1);
            diff_ave_balance += (std::abs(group_ave - aves[i]) / Group::N) * value_ave_params[i];
            group_ave = (double)(next_group.get_sum_values()[i] + item.values[i]) / (next_group.get_member_num() + 1);
            diff_ave_balance += (std::abs(group_ave - aves[i]) / Group::N) * value_ave_params[i];
        }
    }
    //std::cerr << "v_sum" << std::endl;
    if (eval_flags.test(EvalIdx::VALUE_SUM)) {
        if (diff_group_num != 0) {
            diff_sum_balance = -sum_balance;
            for (size_t i = 0; i < Group::N; ++i) {
                if (i == now_group.get_id() || i == next_group.get_id()) continue;
                for (size_t j = 0; j < Item::v_size; ++j) {
                    diff_sum_balance += std::abs(groups[i].get_sum_values()[j] - sum_values[j] / (valid_groups.size() + diff_group_num)) * value_sum_params[j];
                }
            }
            for (size_t i = 0; i < Item::v_size; ++i) {
                diff_sum_balance += std::abs(now_group.get_sum_values()[i] - item.values[i] - sum_values[i] / (valid_groups.size() + diff_group_num)) * value_sum_params[i];
                diff_sum_balance += std::abs(next_group.get_sum_values()[i] + item.values[i] - sum_values[i] / (valid_groups.size() + diff_group_num)) * value_sum_params[i];
            }
        }
        else {
            for (size_t i = 0; i < Item::v_size; ++i) {
                diff_sum_balance -= std::abs(now_group.get_sum_values()[i] - sum_values[i] / valid_groups.size()) * value_sum_params[i];
                diff_sum_balance -= std::abs(next_group.get_sum_values()[i] - sum_values[i] / valid_groups.size()) * value_sum_params[i];
                diff_sum_balance += std::abs(now_group.get_sum_values()[i] - item.values[i] - sum_values[i] / valid_groups.size()) * value_sum_params[i];
                diff_sum_balance += std::abs(next_group.get_sum_values()[i] + item.values[i] - sum_values[i] / valid_groups.size()) * value_sum_params[i];
            }
        }
    }
    //std::cerr << "shi_end" << std::endl;

    //計算誤差の対策
    if (std::abs(diff_penalty) < 1e-10) diff_penalty = 0;
    if (std::abs(diff_relation) < 1e-10) diff_relation = 0;
    if (std::abs(diff_ave_balance) < 1e-10) diff_ave_balance = 0;
    if (std::abs(diff_sum_balance) < 1e-10) diff_sum_balance = 0;
    
    return {diff_penalty, diff_relation, diff_ave_balance, diff_sum_balance, diff_group_num};
}

/*swap移動時の評価値の変化量を計算*/
auto Solution::evaluation_swap(const Item& item1, const Item& item2) -> std::tuple<double, double, double, double, int> {
    const Group& g1 = groups[item_group_ids[item1.id]];
    const Group& g2 = groups[item_group_ids[item2.id]];

    //ペナルティの差分を計算
    double diff_penalty = 0;
    if (eval_flags.test(EvalIdx::WEIGHT_PENA)) {
        diff_penalty += g1.diff_weight_penalty({&item2}, {&item1});
        diff_penalty += g2.diff_weight_penalty({&item1}, {&item2});
    }
    
    if (eval_flags.test(EvalIdx::ITEM_PENA)) {
        diff_penalty += get_each_group_item_penalty(item1, g2.get_id());
        diff_penalty += get_each_group_item_penalty(item2, g1.get_id());
        diff_penalty -= get_each_group_item_penalty(item1, g1.get_id());
        diff_penalty -= get_each_group_item_penalty(item2, g2.get_id());
        diff_penalty -= item1.item_penalty[item2.id] * 2;
    }

    if (eval_flags.test(EvalIdx::GROUP_PENA)) {
        diff_penalty += item1.group_penalty[g2.get_id()];
        diff_penalty += item2.group_penalty[g1.get_id()];
        diff_penalty -= item1.group_penalty[g1.get_id()];
        diff_penalty -= item2.group_penalty[g2.get_id()];
    }

    //関係値の差分を計算
    double diff_relation = 0;
    if (eval_flags.test(EvalIdx::ITEM_R)) {
        for (size_t i = 0; i < Item::item_r_size; ++i) {
            diff_relation += get_each_group_item_relation(item1, g2.get_id())[i];
            diff_relation += get_each_group_item_relation(item2, g1.get_id())[i];
            diff_relation -= get_each_group_item_relation(item1, g1.get_id())[i];
            diff_relation -= get_each_group_item_relation(item2, g2.get_id())[i];

            diff_relation -= item1.item_relations[item2.id][i] * item_relation_params[i] * 2;
        }
    }

    if (eval_flags.test(EvalIdx::GROUP_R)) {
        for (size_t i = 0; i < Item::group_r_size; ++i) {
            diff_relation += item1.group_relations[g2.get_id()][i] * group_relation_params[i];
            diff_relation += item2.group_relations[g1.get_id()][i] * group_relation_params[i];
            diff_relation -= item1.group_relations[g1.get_id()][i] * group_relation_params[i];
            diff_relation -= item2.group_relations[g2.get_id()][i] * group_relation_params[i];
        }
    }

    //ave_balanceとsum_balanceの差分を計算
    double diff_ave_balance = 0, diff_sum_balance = 0;
    std::bitset<8> mask = (1<<EvalIdx::VALUE_AVE) | (1<<EvalIdx::VALUE_SUM);

    if ((eval_flags & mask).any()) {
        for (size_t i = 0; i < Item::v_size; ++i) {
            if (eval_flags.test(EvalIdx::VALUE_AVE)) {
                diff_ave_balance -= (std::abs(g1.value_average(i) - aves[i]) / Group::N) * value_ave_params[i];
                diff_ave_balance -= (std::abs(g2.value_average(i) - aves[i]) / Group::N) * value_ave_params[i];

                double group_ave = (double)(g1.get_sum_values()[i] - item1.values[i] + item2.values[i]) / g1.get_member_num();
                diff_ave_balance += (std::abs(group_ave - aves[i]) / Group::N) * value_ave_params[i];
                group_ave = (double)(g2.get_sum_values()[i] - item2.values[i] + item1.values[i]) / g2.get_member_num();
                diff_ave_balance += (std::abs(group_ave - aves[i]) / Group::N) * value_ave_params[i];
            }
            
            if (eval_flags.test(EvalIdx::VALUE_SUM)) {
                diff_sum_balance -= std::abs(g1.get_sum_values()[i] - sum_values[i] / Group::N) * value_sum_params[i];
                diff_sum_balance -= std::abs(g2.get_sum_values()[i] - sum_values[i] / Group::N) * value_sum_params[i];
                diff_sum_balance += std::abs(g1.get_sum_values()[i] - item1.values[i] + item2.values[i] - sum_values[i] / Group::N) * value_sum_params[i];
                diff_sum_balance += std::abs(g2.get_sum_values()[i] - item2.values[i] + item1.values[i] - sum_values[i] / Group::N) * value_sum_params[i];
            }
        }
    }

    //計算誤差の対策
    if (std::abs(diff_penalty) < 1e-10) diff_penalty = 0;
    if (std::abs(diff_relation) < 1e-10) diff_relation = 0;
    if (std::abs(diff_ave_balance) < 1e-10) diff_ave_balance = 0;
    if (std::abs(diff_sum_balance) < 1e-10) diff_sum_balance = 0;
    
    return {diff_penalty, diff_relation, diff_ave_balance, diff_sum_balance, 0};
}

/*移動処理*/
void Solution::move_processing(const std::vector<MoveItem>& move_items, const std::tuple<double, double, double, double, int>& diff) {
    auto [diff_penalty, diff_relation, diff_ave_balance, diff_sum_balance, diff_group_num] = diff;
    set_eval_value(penalty + diff_penalty, relation + diff_relation, ave_balance + diff_ave_balance, sum_balance + diff_sum_balance);

    //値の更新
    for (const auto& mi : move_items) {
        for (int i = 0; i < Item::N; ++i) {
            if (each_group_item_relation[i][mi.source] && mi.source < Group::N) {
                for (size_t j = 0; j < Item::item_r_size; ++j) {
                    each_group_item_relation[i][mi.source].value()[j] -= mi.item.item_relations[i][j] * item_relation_params[j];
                }
            }
            if (each_group_item_relation[i][mi.destination] && mi.destination < Group::N) {
                for (size_t j = 0; j < Item::item_r_size; ++j) {
                    each_group_item_relation[i][mi.destination].value()[j] += mi.item.item_relations[i][j] * item_relation_params[j];
                }
            }
            if (each_group_item_penalty[i][mi.source] && mi.source < Group::N) {
                each_group_item_penalty[i][mi.source].value() -= mi.item.item_penalty[i];
            }
            if (each_group_item_penalty[i][mi.destination] && mi.destination < Group::N) {
                each_group_item_penalty[i][mi.destination].value() += mi.item.item_penalty[i];
            }
         }
    }

    //移動
    for (const auto& mi : move_items) {
        //std::cerr << mi.item.id << " " << mi.source << " " << mi.destination << std::endl;
        assert(item_group_ids[mi.item.id] == mi.source);
        groups[mi.source].erase_member(mi.item);
        if (eval_flags.test(EvalIdx::GROUP_NUM)) {
            if (groups[mi.destination].get_member_num() == 0 && mi.destination != Group::N) {
                valid_groups.emplace_back(std::make_unique<const Group>(groups[mi.destination]));
            }
        }
        groups[mi.destination].add_member(mi.item);
        item_group_ids[mi.item.id] = mi.destination;
    }
    for (auto&& group : groups) {
        //std::cerr << group << std::endl;
    }

    if (eval_flags.test(EvalIdx::GROUP_NUM)) {
        for (auto g_itr = valid_groups.begin(), end = valid_groups.end(); g_itr != end; ++g_itr) {
            if ((*g_itr)->get_member_num() == 0) g_itr = valid_groups.erase(g_itr);
        }
    }
}

/*
 *shift移動するかどうかを調査し, 必要に応じて移動する
 *移動した場合はtrue, していない場合はfalseを返す
 */
bool Solution::shift_check(const Item& item, int group_id) {
    auto diff = evaluation_shift(item, group_id);
    double increace = calc_diff_eval(diff);
    if (increace > 0 && std::abs(increace) > 1e-10) {
        move_processing({MoveItem(item, item_group_ids[item.id], group_id)}, diff);
        return true;
    }
    return false;
}

/*
 *swap移動するかどうかを調査し, 必要に応じて移動する
 *移動した場合はtrue, していない場合はfalseを返す
 */
bool Solution::swap_check(const Item& item1, const Item& item2) {
    auto diff = evaluation_swap(item1, item2);
    double increace = calc_diff_eval(diff);
    if (increace > 0 && std::abs(increace) > 1e-10) {
        int g1_id = item_group_ids[item1.id], g2_id = item_group_ids[item2.id];
        move_processing({MoveItem(item1, g1_id, g2_id), MoveItem(item2, g2_id, g1_id)}, diff);
        return true;
    }
    return false;
}

/*
 *move_itemsに基づいて移動するかどうかを調査し, 必要に応じて移動する
 *移動した場合はtrue, していない場合はfalseを返す
 */
bool Solution::move_check(const vector<MoveItem>& move_items) {
    auto diff = evaluation_diff(move_items);
    double increace = calc_diff_eval(diff);

    if (increace > 0) {
        move_processing(move_items, diff);
        return true;
    }
    return false;
}

/*move_itemsに基づいて移動する*/
void Solution::move(const vector<MoveItem>& move_items) {
    move_processing(move_items, evaluation_diff(move_items));
}

/*解の出力用*/
std::ostream& operator<<(std::ostream& out, const Solution& s) {
    for (auto&& group : s.get_valid_groups()) {
        out << *group << std::endl;
    }
    /*for (auto&& g : s.groups) {
        out << g << std::endl;
    }*/
    out << "評価値:";
    if (s.opt == Input::Opt::MAX) {
        out << s.get_eval_value();
    }
    else {
        out << -s.get_eval_value();
    }
    out << std::endl;
    return out;
}