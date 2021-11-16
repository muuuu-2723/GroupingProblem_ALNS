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
#include <set>

using std::vector;

/*�R���X�g���N�^*/
Solution::Solution(const Input& input) {
    //�p�����[�^�ݒ�
    penalty_param = 100000;
    opt = input.get_opt();
    item_relation_params = input.get_item_relation_params();
    group_relation_params = input.get_group_relation_params();
    value_ave_params = input.get_value_ave_params();
    value_sum_params = input.get_value_sum_params();
    //group_cost = input.get_group_cost();
    group_cost = vector<double>(input.get_group_cost().begin(), input.get_group_cost().end());
    constant = input.get_constant();

    //�ŏ����̏ꍇ�̓p�����[�^�̕����𔽓]
    if (opt == Input::Opt::MIN) {
        std::for_each(item_relation_params.begin(), item_relation_params.end(), [](auto& p) { p *= -1; });
        std::for_each(group_relation_params.begin(), group_relation_params.end(), [](auto& p) { p *= -1; });
        std::for_each(value_ave_params.begin(), value_ave_params.end(), [](auto& p) { p *= -1; });
        std::for_each(value_sum_params.begin(), value_sum_params.end(), [](auto& p) { p *= -1; });
        std::for_each(group_cost.begin(), group_cost.end(), [](auto& p) { p *= -1; });
        constant *= -1;
    }

    //eval_flags�̐ݒ�
    eval_flags.set();
    if (Item::w_size == 0) eval_flags.reset(EvalIdx::WEIGHT_PENA);
    if (input.get_item_penalty_num() == 0) eval_flags.reset(EvalIdx::ITEM_PENA);
    if (input.get_group_penalty_num() == 0) eval_flags.reset(EvalIdx::GROUP_PENA);
    if (item_relation_params.size() == 0) eval_flags.reset(EvalIdx::ITEM_R);
    if (group_relation_params.size() == 0) eval_flags.reset(EvalIdx::GROUP_R);
    if (value_ave_params.size() == 0) eval_flags.reset(EvalIdx::VALUE_AVE);
    if (value_sum_params.size() == 0) eval_flags.reset(EvalIdx::VALUE_SUM);
    if (group_cost.size() == 0) eval_flags.reset(EvalIdx::GROUP_COST);
    std::cout << eval_flags << std::endl;

    Group::weight_aves.reserve(Item::w_size);
    for (size_t i = 0; i < Item::w_size; ++i) {
        double weight_sum = 0;
        int cnt = 0;
        for (auto&& item : input.get_items()) {
            weight_sum += item.weight[i];
            if (item.weight[i] != 0) ++cnt;
        }
        Group::weight_aves.push_back(weight_sum / cnt);
    }
    groups.clear();
    valid_groups.clear();
    groups.reserve(Group::N + 1);
    for (size_t i = 0; i < Group::N; ++i) {
        groups.push_back(Group(i, input.get_weight_upper(i), input.get_weight_lower(i)));
        if (!eval_flags.test(EvalIdx::GROUP_COST)) {
            valid_groups.push_back(&groups[i]);
        }
    }
    groups.push_back(Group(Group::N));
    each_group_item_relation.assign(Item::N, vector<std::optional<vector<double>>>(Group::N + 1, std::nullopt));
    each_group_item_penalty.assign(Item::N, vector<std::optional<int>>(Group::N + 1, std::nullopt));
    item_group_ids.resize(Item::N);
    aves.assign(Item::v_size, 0);
    sum_values.assign(Item::v_size, 0);

    //�w��O���[�v������A�C�e�����w��O���[�v��
    //����ȊO�̃A�C�e���̓_�~�[�O���[�v��
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
    
    item_times.resize(Item::N, vector<int>(Item::N, 0));
    group_times.resize(Item::N, vector<int>(Group::N, 0));
    //relation_greedy(items);
    

    //penalty_greedy(items);
    //score_greedy(items);
    //RelationGreedy rg(input.get_items(), 1, 1);
    WeightPenaltyGreedy wp(input.get_items(), 1, 1);
    //PenaltyGreedy rg(items, 1);
    std::shared_ptr<Destroy> des = std::make_shared<Destroy>(input.get_items(), 1, 1);
    *this = *wp(*this, des);
}

Solution::Solution(const Solution& s) : groups(s.groups), item_group_ids(s.item_group_ids), relation(s.relation), penalty(s.penalty), ave_balance(s.ave_balance),
                                        sum_balance(s.sum_balance),sum_group_cost(s.sum_group_cost) , each_group_item_relation(s.each_group_item_relation), each_group_item_penalty(s.each_group_item_penalty),
                                        aves(s.aves), sum_values(s.sum_values), opt(s.opt), item_relation_params(s.item_relation_params), group_relation_params(s.group_relation_params),
                                        value_ave_params(s.value_ave_params), value_sum_params(s.value_sum_params), penalty_param(s.penalty_param), group_cost(s.group_cost),
                                        constant(s.constant), eval_flags(s.eval_flags), item_times(s.item_times), group_times(s.group_times) {

    //groups = s.groups;
    for (auto&& g_ptr : s.valid_groups) {
        valid_groups.push_back(&groups[g_ptr->get_id()]);
    }
    //std::cerr << "�R�s�[�R���X�g���N�^" << std::endl;
}

/*each_group_item_relation�̒l���擾, �Ȃ���Όv�Z���Ď擾*/
const vector<double>& Solution::get_each_group_item_relation(const Item& item, int group_id) {
    if (group_id >= Group::N) {
        each_group_item_relation[item.id][group_id] = vector<double>(Item::item_r_size + Item::v_size, 0);
    }
    else if (!each_group_item_relation[item.id][group_id]) {
        each_group_item_relation[item.id][group_id] = groups[group_id].item_relation(item, item_relation_params);
    }
    return each_group_item_relation[item.id][group_id].value();
}

/*each_group_item_penalty�̒l���擾, �Ȃ���Όv�Z���Ď擾*/
int Solution::get_each_group_item_penalty(const Item& item, int group_id) {
    if (group_id >= Group::N) {
        each_group_item_penalty[item.id][group_id] = 0;
    }
    else if (!each_group_item_penalty[item.id][group_id]) {
        each_group_item_penalty[item.id][group_id] = groups[group_id].calc_item_penalty(item);
    }
    return each_group_item_penalty[item.id][group_id].value();
}

/*���݂̉�(�O���[�v����)��]��*/
double Solution::evaluation_all(const vector<Item>& items) {
    penalty = 0;
    relation = 0;
    ave_balance = 0;
    sum_balance = 0;
    sum_group_cost = 0;
    auto [group_begin, group_end] = get_groups_range();
    for (auto g_itr = group_begin; g_itr != group_end; ++g_itr) {
        //�y�i���e�B�v�Z
        if (eval_flags.test(EvalIdx::WEIGHT_PENA)) penalty += g_itr->calc_weight_penalty();
        if (eval_flags.test(EvalIdx::ITEM_PENA)) penalty += g_itr->calc_sum_item_penalty(items);
        if (eval_flags.test(EvalIdx::GROUP_PENA)) penalty += g_itr->calc_group_penalty(items);

        //�֌W�l�̌v�Z
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

        //value�̕������p
        if (eval_flags.test(EvalIdx::VALUE_AVE)) {
            for (size_t i = 0; i < Item::v_size; ++i) {
                ave_balance += std::abs(aves[i] - g_itr->value_average(i)) * value_ave_params[i];
            }
        }
        if (eval_flags.test(EvalIdx::VALUE_SUM)) {
            for (size_t i = 0; i < Item::v_size; ++i) {
                if (eval_flags.test(EvalIdx::GROUP_COST)) {
                    sum_balance += std::abs(sum_values[i] / valid_groups.size() - g_itr->get_sum_values()[i]) * value_sum_params[i];
                }
                else {
                    sum_balance += std::abs(sum_values[i] / Group::N - g_itr->get_sum_values()[i]) * value_sum_params[i];
                }
            }
        }

        //�O���[�v�R�X�g�̌v�Z
        if (eval_flags.test(EvalIdx::GROUP_COST)) {
            if (g_itr->get_member_num() != 0) sum_group_cost += group_cost[g_itr->get_id()];
        }
    }
    ave_balance /= Group::N;

    return get_eval_value();
}

/*�]���l�̕ω��ʂ��v�Z*/
auto Solution::evaluation_diff(const vector<MoveItem>& move_items) -> std::tuple<double, double, double, double, double> {
    //std::cerr << "eval_diff" << std::endl;
    vector<vector<const Item*>> in(Group::N);
    vector<vector<const Item*>> out(Group::N);
    for (const auto& mi : move_items) {
        if (mi.source < Group::N) out[mi.source].push_back(&mi.item);
        if (mi.destination < Group::N) in[mi.destination].push_back(&mi.item);
    }
    //std::cerr << "eval_diff" << std::endl;
    //�O���[�v���̑������v�Z
    double diff_group_cost = 0;
    int diff_group_num = 0;
    if (eval_flags.test(EvalIdx::GROUP_COST)) {
        for (size_t i = 0; i < Group::N; ++i) {
            int member_num = groups[i].get_member_num();
            if (member_num == 0) {
                if (in[i].size() != 0) {
                    diff_group_cost += group_cost[i];
                    ++diff_group_num;
                }
            }
            else {
                if (member_num - out[i].size() + in[i].size() == 0) {
                    diff_group_cost -= group_cost[i];
                    --diff_group_num;
                }
            }
        }
    }
    //std::cerr << "eval_diff" << std::endl;
    //�y�i���e�B�̍������v�Z
    double diff_penalty = 0;
    if (eval_flags.test(EvalIdx::WEIGHT_PENA)) {
        for (size_t i = 0; i < Group::N; ++i) {
            double tmp = groups[i].diff_weight_penalty(in[i], out[i]);
            diff_penalty += tmp;
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
    //�֌W�l�̍������v�Z
    double diff_relation = 0;
    if (eval_flags.test(EvalIdx::ITEM_R)) {
        for (size_t i = 0; i < Item::item_r_size + Item::v_size; ++i) {
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
    //ave_balance��sum_balance�̍������v�Z
    double diff_ave_balance = 0, diff_sum_balance = 0;
    std::bitset<8> mask = (1<<EvalIdx::VALUE_AVE) | (1<<EvalIdx::VALUE_SUM);
    std::bitset<8> mask2 = (1<<EvalIdx::VALUE_SUM) | (1<<EvalIdx::GROUP_COST);
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
                    if (eval_flags.test(EvalIdx::GROUP_COST)) {
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
    //�v�Z�덷�̑΍�
    if (std::abs(diff_penalty) < 1e-10) diff_penalty = 0;
    if (std::abs(diff_relation) < 1e-10) diff_relation = 0;
    if (std::abs(diff_ave_balance) < 1e-10) diff_ave_balance = 0;
    if (std::abs(diff_sum_balance) < 1e-10) diff_sum_balance = 0;
    if (std::abs(diff_group_cost) < 1e-10) diff_group_cost = 0;
    
    return {diff_penalty, diff_relation, diff_ave_balance, diff_sum_balance, diff_group_cost};
}

/*shift�ړ����̕]���l�̕ω��ʂ��v�Z*/
auto Solution::evaluation_shift(const Item& item, int group_id) -> std::tuple<double, double, double, double, double> {
    //std::cerr << "eval_shi" << std::endl;
    const Group& now_group = groups[item_group_ids[item.id]];
    const Group& next_group = groups[group_id];
    //std::cerr << "g_num" << std::endl;
    //�O���[�v���̑������v�Z
    int diff_group_num = 0;
    double diff_group_cost = 0;
    if (eval_flags.test(EvalIdx::GROUP_COST)) {
        if (now_group.get_member_num() == 1) {
            --diff_group_num;
            diff_group_cost -= group_cost[now_group.get_id()];
        }
        if (next_group.get_member_num() == 0) {
            ++diff_group_num;
            diff_group_cost += group_cost[next_group.get_id()];
        }
    }

    //�y�i���e�B�̍������v�Z
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

    //�֌W�l�̍������v�Z
    double diff_relation = 0;
    //std::cerr << "i_r" << std::endl;
    if (eval_flags.test(EvalIdx::ITEM_R)) {
        for (size_t i = 0; i < Item::item_r_size + Item::v_size; ++i) {
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

    //ave_balance��sum_balance�̍������v�Z
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

    //�v�Z�덷�̑΍�
    if (std::abs(diff_penalty) < 1e-10) diff_penalty = 0;
    if (std::abs(diff_relation) < 1e-10) diff_relation = 0;
    if (std::abs(diff_ave_balance) < 1e-10) diff_ave_balance = 0;
    if (std::abs(diff_sum_balance) < 1e-10) diff_sum_balance = 0;
    if (std::abs(diff_group_cost) < 1e-10) diff_group_cost = 0;
    
    return {diff_penalty, diff_relation, diff_ave_balance, diff_sum_balance, diff_group_cost};
}

/*swap�ړ����̕]���l�̕ω��ʂ��v�Z*/
auto Solution::evaluation_swap(const Item& item1, const Item& item2) -> std::tuple<double, double, double, double, double> {
    const Group& g1 = groups[item_group_ids[item1.id]];
    const Group& g2 = groups[item_group_ids[item2.id]];

    //�y�i���e�B�̍������v�Z
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

    //�֌W�l�̍������v�Z
    double diff_relation = 0;
    if (eval_flags.test(EvalIdx::ITEM_R)) {
        for (size_t i = 0; i < Item::item_r_size + Item::v_size; ++i) {
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

    //ave_balance��sum_balance�̍������v�Z
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

    //�v�Z�덷�̑΍�
    if (std::abs(diff_penalty) < 1e-10) diff_penalty = 0;
    if (std::abs(diff_relation) < 1e-10) diff_relation = 0;
    if (std::abs(diff_ave_balance) < 1e-10) diff_ave_balance = 0;
    if (std::abs(diff_sum_balance) < 1e-10) diff_sum_balance = 0;
    
    return {diff_penalty, diff_relation, diff_ave_balance, diff_sum_balance, 0};
}

/*�ړ�����*/
void Solution::move_processing(const std::vector<MoveItem>& move_items, const std::tuple<double, double, double, double, double>& diff) {
    auto [diff_penalty, diff_relation, diff_ave_balance, diff_sum_balance, diff_group_cost] = diff;
    set_eval_value(penalty + diff_penalty, relation + diff_relation, ave_balance + diff_ave_balance, sum_balance + diff_sum_balance, sum_group_cost + diff_group_cost);

    std::set<int> add_id;
    //�l�̍X�V
    for (const auto& mi : move_items) {
        for (int i = 0; i < Item::N; ++i) {
            if (each_group_item_relation[i][mi.source] && mi.source < Group::N) {
                for (size_t j = 0; j < Item::item_r_size + Item::v_size; ++j) {
                    each_group_item_relation[i][mi.source].value()[j] -= mi.item.item_relations[i][j] * item_relation_params[j];
                }
            }
            if (each_group_item_relation[i][mi.destination] && mi.destination < Group::N) {
                for (size_t j = 0; j < Item::item_r_size + Item::v_size; ++j) {
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
        if (eval_flags.test(EvalIdx::GROUP_COST)) {
            if (groups[mi.destination].get_member_num() == 0 && mi.destination != Group::N) {
                add_id.insert(mi.destination);
            }
        }
    }

    //�ړ�
    for (const auto& mi : move_items) {
        //std::cerr << mi.item.id << " " << mi.source << " " << mi.destination << std::endl;
        assert(item_group_ids[mi.item.id] == mi.source);
        groups[mi.source].erase_member(mi.item);
        groups[mi.destination].add_member(mi.item);
        item_group_ids[mi.item.id] = mi.destination;
    }
    if (eval_flags.test(EvalIdx::GROUP_COST)) {
        for (auto&& id : add_id) {
            valid_groups.push_back(&groups[id]);
        }
        for (auto g_itr = valid_groups.begin(); g_itr != valid_groups.end();) {
            if ((*g_itr)->get_member_num() == 0) g_itr = valid_groups.erase(g_itr);
            else ++g_itr;
        }
    }
}

/*
 *shift�ړ����邩�ǂ����𒲍���, �K�v�ɉ����Ĉړ�����
 *�ړ������ꍇ��true, ���Ă��Ȃ��ꍇ��false��Ԃ�
 */
bool Solution::shift_check(const Item& item, int group_id) {
    auto diff = evaluation_shift(item, group_id);
    double increace = calc_diff_eval(diff);
    if (increace >= 0 && std::abs(increace) > 1e-10) {
        move_processing({MoveItem(item, item_group_ids[item.id], group_id)}, diff);
        return true;
    }
    return false;
}

/*
 *swap�ړ����邩�ǂ����𒲍���, �K�v�ɉ����Ĉړ�����
 *�ړ������ꍇ��true, ���Ă��Ȃ��ꍇ��false��Ԃ�
 */
bool Solution::swap_check(const Item& item1, const Item& item2) {
    auto diff = evaluation_swap(item1, item2);
    double increace = calc_diff_eval(diff);
    if (increace >= 0 && std::abs(increace) > 1e-10) {
        int g1_id = item_group_ids[item1.id], g2_id = item_group_ids[item2.id];
        move_processing({MoveItem(item1, g1_id, g2_id), MoveItem(item2, g2_id, g1_id)}, diff);
        return true;
    }
    return false;
}

/*
 *move_items�Ɋ�Â��Ĉړ����邩�ǂ����𒲍���, �K�v�ɉ����Ĉړ�����
 *�ړ������ꍇ��true, ���Ă��Ȃ��ꍇ��false��Ԃ�
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

/*move_items�Ɋ�Â��Ĉړ�����*/
void Solution::move(const vector<MoveItem>& move_items) {
    move_processing(move_items, evaluation_diff(move_items));
}

void Solution::shift_move(const Item& item, int group_id) {
    move_processing({MoveItem(item, item_group_ids[item.id], group_id)}, evaluation_shift(item, group_id));
}

void Solution::swap_move(const Item& item1, const Item& item2) {
    int g1_id = item_group_ids[item1.id], g2_id = item_group_ids[item2.id];
    move_processing({MoveItem(item1, g1_id, g2_id), MoveItem(item2, g2_id, g1_id)}, evaluation_swap(item1, item2));
}

/*���̏o�͗p*/
std::ostream& operator<<(std::ostream& out, const Solution& s) {
    for (auto&& group : s.get_valid_groups()) {
        out << *group << std::endl;
    }
    /*for (auto&& g : s.groups) {
        out << g << std::endl;
    }*/
    out << "�]���l:";
    if (s.opt == Input::Opt::MAX) {
        out << s.get_eval_value();
    }
    else {
        out << -s.get_eval_value();
    }
    out << std::endl;
    return out;
}

void Solution::counter() {
    for (auto&& group_ptr : valid_groups) {
        auto& member_list = group_ptr->get_member_list();
        for (auto m_itr1 = member_list.begin(), end = member_list.end(); m_itr1 != end; ++m_itr1) {
            ++group_times[*m_itr1][group_ptr->get_id()];
            for (auto m_itr2 = std::next(m_itr1); m_itr2 != end; ++m_itr2) {
                ++item_times[*m_itr1][*m_itr2];
                ++item_times[*m_itr2][*m_itr1];
            }
        }
    }
}