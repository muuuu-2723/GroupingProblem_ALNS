#include "GroupPenaltyGreedy.hpp"
#include <Solution.hpp>
#include <Destroy.hpp>
#include <Item.hpp>
#include <vector>
#include <memory>
#include <climits>

using std::vector;

/*
 *貪欲法で新たな解を生成
 *destroy_ptrで解を破壊し, アイテムとグループのペナルティが少ないグループに割り当てる
 */
Solution GroupPenaltyGreedy::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    std::unique_ptr<Solution> best;                                                 //生成した解で一番良い評価値の解
    for (size_t i = 0; i < 40; ++i) {
        //現在の解をコピーし, それを破壊
        std::unique_ptr<Solution> neighborhood(new Solution(current_solution));
        (*destroy_ptr)(*neighborhood);

        //破壊されたアイテム(ダミーグループ)をgroup_penaltyが最も少ないグループに割り当て
        auto& dummy_group = neighborhood->get_dummy_group();
        vector<MoveItem> move_items;
        move_items.reserve(dummy_group.get_member_num());
        for (auto&& id : dummy_group.get_member_list()) {
            int assign_group_id;
            int min_penalty = INT_MAX;
            auto [group_begin, group_end] = neighborhood->get_groups_range();
            for (auto g_itr = group_begin; g_itr != group_end; ++g_itr) {
                int penalty = 0;
                if (neighborhood->get_eval_flags().test(Solution::EvalIdx::GROUP_PENA)) {
                    penalty += items[id].group_penalty[g_itr->get_id()];
                }
                if (min_penalty > penalty) {
                    min_penalty = penalty;
                    assign_group_id = g_itr->get_id();
                }
            }
            move_items.push_back(MoveItem(items[id], neighborhood->get_group_id(items[id]), assign_group_id));
        }
        neighborhood->move(move_items);

        if (!best || best->get_eval_value() < neighborhood->get_eval_value()) {
            best = std::move(neighborhood);
        }
    }

    return std::move(*best);
}