#include "GroupPenaltyGreedy.hpp"
#include <Solution.hpp>
#include <Destroy.hpp>
#include <Item.hpp>
#include <MyRandom.hpp>
#include <vector>
#include <memory>
#include <climits>
#include <numeric>

using std::vector;

/*
 *貪欲法で新たな解を生成
 *destroy_ptrで解を破壊し, アイテムとグループのペナルティが少ないグループに割り当てる
 */
std::unique_ptr<Solution> GroupPenaltyGreedy::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    std::unique_ptr<Solution> best;                                                 //生成した解で一番良い評価値の解
    //std::cout << "gp_test" << std::endl;
    //std::cout << current_solution << std::endl;
    for (size_t i = 0; i < /*40*/10; ++i) {
        //現在の解をコピーし, それを破壊
        auto neighborhood = std::make_unique<Solution>(current_solution);
        auto destroy_items = (*destroy_ptr)(*neighborhood);
        vector<MoveItem> destroy_move;
        destroy_move.reserve(destroy_items.size());
        for (auto&& item : destroy_items) {
            destroy_move.push_back(MoveItem(*item, neighborhood->get_group_id(*item), neighborhood->get_dummy_group().get_id()));
        }
        neighborhood->move(destroy_move);

        //破壊されたアイテム(ダミーグループ)をgroup_penaltyが最も少ないグループに割り当て
        auto& dummy_group = neighborhood->get_dummy_group();
        vector<MoveItem> move_items;
        move_items.reserve(dummy_group.get_member_num());
        auto [group_begin, group_end] = neighborhood->get_groups_range();
        vector<size_t> shuffle_group_ids(std::distance(group_begin, group_end));
        std::iota(shuffle_group_ids.begin(), shuffle_group_ids.end(), 0);
        MyRandom::shuffle(shuffle_group_ids);
        for (auto&& id : dummy_group.get_member_list()) {
            int assign_group_id;
            int min_penalty = INT_MAX;
            for (auto&& g_id : shuffle_group_ids) {
                auto g_itr = group_begin + g_id;
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
            is_move = (move_items.size() > 0);
        }
    }
    //std::cout << *best << std::endl;

    return std::move(best);
}