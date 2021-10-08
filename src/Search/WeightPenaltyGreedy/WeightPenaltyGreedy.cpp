#include "WeightPenaltyGreedy.hpp"
#include <Solution.hpp>
#include <Group.hpp>
#include <Destroy.hpp>
#include <Item.hpp>
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

/*
 *貪欲法で新たな解を生成
 *destroy_ptrで解を破壊し, 下限に足りていないグループに優先してアイテムを割り当てる
 *その後, 上限を超えないグループに割り当てていく
 *関係あるアイテム(weight[type]!=0)が少ないtypeのweightを優先する
 */
Solution WeightPenaltyGreedy::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    std::unique_ptr<Solution> best;                                                 //生成した解で一番良い評価値の解
    size_t N = 7/*13*/;

    //ランダム破壊でない場合, 繰り返す必要がないためN = 1にする
    if (typeid(*destroy_ptr) == typeid(MinimumDestroy) || typeid(*destroy_ptr) == typeid(MinimumGroupDestroy)) {
        N = 1;
    }

    for (size_t i = 0; i < N; ++i) {
        //現在の解をコピーし, それを破壊
        std::unique_ptr<Solution> neighborhood(new Solution(current_solution));
        (*destroy_ptr)(*neighborhood);

        const Group& dummy_group = neighborhood->get_dummy_group();
        vector<MoveItem> move_items;
        move_items.reserve(dummy_group.get_member_num());
        //すでに割り当てたアイテムを削除していくためコピーをとる
        auto member_list = dummy_group.get_member_list();

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
        auto& lower = Group::get_lower();
        for (auto [type, cnt] : priority_type) {
            for (auto m_itr = member_list.begin(); m_itr != member_list.end(); ++m_itr) {
                const Item& item = items[*m_itr];
                auto [group_begin, group_end] = neighborhood->get_groups_range();
                for (auto g_itr = group_begin; g_itr != group_end; ++g_itr) {
                    if (item.weight[type] != 0 && g_itr->get_sum_weight()[type] < lower[type]) {
                        neighborhood->move({MoveItem(item, neighborhood->get_group_id(item), g_itr->get_id())});
                        m_itr = member_list.erase(m_itr);
                    }
                }
            }
        }

        //残りのアイテムを上限を超えないグループに割り当てる
        vector<vector<const Item*>> add_members(Group::N);
        for (const auto& id : member_list) {
            int min_group_id = -1;
            int min_penalty = INT_MAX;
            auto [group_begin, group_end] = neighborhood->get_groups_range();
            for (auto g_itr = group_begin; g_itr != group_end; ++g_itr) {
                vector<const Item*> tmp = add_members[g_itr->get_id()];
                tmp.push_back(&items[id]);
                int diff_penalty = g_itr->diff_weight_penalty(tmp, {}) - g_itr->diff_weight_penalty(add_members[g_itr->get_id()], {});
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
        }
    }
    return std::move(*best);
}