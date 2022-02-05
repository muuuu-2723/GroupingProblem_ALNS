#include "WeightPenaltyGreedy.hpp"
#include <Solution.hpp>
#include <Group.hpp>
#include <Destroy.hpp>
#include <Item.hpp>
#include <MyRandom.hpp>
#include <memory>
#include <vector>
#include <climits>
#include <iostream>
#include <list>
#include <typeinfo>
#include <numeric>
#include <utility>
#include <algorithm>

using std::vector;

WeightPenaltyGreedy::WeightPenaltyGreedy(const vector<Item>& items, double init_weight, int param, const Solution& solution) : Search(items, init_weight, param, solution) {
    destructions.emplace_back(std::make_shared<RandomDestroy>(items, init_item_destroy_num, 50, 1, solution));
    destructions.emplace_back(std::make_shared<RandomGroupDestroy>(items, init_group_destroy_num, 50, 1, solution));
    destructions.emplace_back(std::make_shared<MinimumDestroy>(items, init_item_destroy_num, 50, 1, solution));
    destructions.emplace_back(std::make_shared<MinimumGroupDestroy>(items, init_group_destroy_num, 50, 1, solution));
    destructions.emplace_back(std::make_shared<UpperWeightGreedyDestroy>(items, init_group_destroy_num, 50, 1, solution));
    
    init_destroy_random();

    for (auto&& d : destructions) {
        if (typeid(*d) == typeid(RandomDestroy) || typeid(*d) == typeid(MinimumDestroy)) {
            item_destroy.emplace_back(d);
        }
        else {
            group_destroy.emplace_back(d);
        }
    }
}

/*
 *貪欲法で新たな解を生成
 *destroy_ptrで解を破壊し, 下限に足りていないグループに優先してアイテムを割り当てる
 *その後, 上限を超えないグループに割り当てていく
 *関係あるアイテム(weight[type]!=0)が少ないtypeのweightを優先する
 */
std::unique_ptr<Solution> WeightPenaltyGreedy::operator()(const Solution& current_solution) {
    std::unique_ptr<Solution> best;                                                 //生成した解で一番良い評価値の解
    size_t N = 10;
    auto& destroy = select_destroy();

    //ランダム破壊でない場合, 繰り返す必要がないためN = 1にする
    /*if (typeid(destroy) == typeid(MinimumDestroy) || typeid(destroy) == typeid(MinimumGroupDestroy)) {
        N = 1;
    }*/
    //std::cout << "wp_test" << std::endl;
    //std::cout << current_solution << std::endl;

    for (size_t i = 0; i < N; ++i) {
        //現在の解をコピーし, それを破壊
        auto neighborhood = std::make_unique<Solution>(current_solution);
        auto destroy_items = destroy(*neighborhood);
        vector<MoveItem> destroy_move;
        destroy_move.reserve(destroy_items.size());
        for (auto&& item : destroy_items) {
            destroy_move.push_back(MoveItem(*item, neighborhood->get_group_id(*item), neighborhood->get_dummy_group().get_id()));
        }
        neighborhood->move(destroy_move);
        //std::cout << "des_finish" << std::endl;

        const Group& dummy_group = neighborhood->get_dummy_group();
        bool is_destroy = (dummy_group.get_member_num() > 0);
        vector<MoveItem> move_items;
        move_items.reserve(dummy_group.get_member_num());
        //すでに割り当てたアイテムを削除していくためコピーをとる
        auto member_list = dummy_group.get_member_list();
        //std::cout << "num:" << dummy_group.get_member_num() << std::endl;

        //typeを関係あるアイテム(weight[type]!=0)が少ない順にする
        vector<std::pair<int, int>> priority_type(Item::w_size);
        for (size_t j = 0; j < Item::w_size; ++j) {
            priority_type[j].first = j;
            int cnt = 0;
            for (const auto& id : member_list) {
                if (items[id].weight[j] == 0) ++cnt;
            }
            priority_type[j].second = cnt;
        }
        std::sort(priority_type.begin(), priority_type.end(), [](const auto& a, const auto& b) { return a.second > b.second; });
        //下限に足りていないグループに優先してアイテムを割り当てる
        for (auto [type, cnt] : priority_type) {
            for (auto m_itr = member_list.begin(); m_itr != member_list.end();) {
                const Item& item = items[*m_itr];
                bool is_assignment = false;
                auto [group_begin, group_end] = neighborhood->get_groups_range();
                for (auto g_itr = group_begin; g_itr != group_end; ++g_itr) {
                    auto& lower = g_itr->get_lower();
                    if (item.weight[type] != 0 && g_itr->get_sum_weight()[type] < lower[type]) {
                        neighborhood->move({MoveItem(item, neighborhood->get_group_id(item), g_itr->get_id())});
                        m_itr = member_list.erase(m_itr);
                        is_assignment = true;
                        break;
                    }
                }
                if (!is_assignment) ++m_itr;
            }
        }
        //残りのアイテムを上限を超えないグループに割り当てる
        auto [group_begin, group_end] = neighborhood->get_groups_range();
        vector<size_t> shuffle_group_ids(std::distance(group_begin, group_end));
        std::iota(shuffle_group_ids.begin(), shuffle_group_ids.end(), 0);
        MyRandom::shuffle(shuffle_group_ids);
        vector<vector<const Item*>> add_members(Group::N);
        for (const auto& id : member_list) {
            //std::cout << "id:" << id << std::endl;
            int min_group_id = -1;
            double min_penalty = DBL_MAX;
            for (auto&& g_id : shuffle_group_ids) {
                auto g_itr = group_begin + g_id;
                vector<const Item*> tmp = add_members[g_itr->get_id()];
                tmp.push_back(&items[id]);
                double diff_penalty = 0;
                if (neighborhood->get_eval_flags().test(Solution::EvalIdx::WEIGHT_PENA)) {
                    diff_penalty += g_itr->diff_weight_penalty(tmp, {}) - g_itr->diff_weight_penalty(add_members[g_itr->get_id()], {});
                }
                //std::cout << "group:" << g_id << ", diff_pena:" << diff_penalty << std::endl;
                if (diff_penalty < min_penalty) {
                    min_penalty = diff_penalty;
                    min_group_id = g_itr->get_id();
                }
            }
            move_items.push_back(MoveItem(items[id], neighborhood->get_group_id(items[id]), min_group_id));
            add_members[min_group_id].push_back(&items[id]);
        }
        neighborhood->move(move_items);
        if (!best || best->get_eval_value() < neighborhood->get_eval_value()) {
            best = std::move(neighborhood);
            is_move = is_destroy;
        }
    }
    //std::cout << *best << std::endl;
    return std::move(best);
}