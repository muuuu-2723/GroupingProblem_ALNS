#include "ItemPenaltyGreedy.hpp"
#include <Solution.hpp>
#include <Destroy.hpp>
#include <Item.hpp>
#include <MyRandom.hpp>
#include <vector>
#include <memory>
#include <climits>

using std::vector;

Solution ItemPenaltyGreedy::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    std::unique_ptr<Solution> best;
    for (size_t i = 0; i < 40; ++i) {
        std::unique_ptr<Solution> neighborhood(new Solution(current_solution));
        (*destroy_ptr)(*neighborhood);

        auto& member_list = neighborhood->get_dummy_group().get_member_list();
        vector<int> target_member(member_list.begin(), member_list.end());
        MyRandom::shuffle(target_member);

        for (const auto& id : target_member) {
            int assign_group_id;
            int min_penalty = INT_MAX;
            auto [group_begin, gorup_end] = neighborhood->get_groups_range();
            for (auto g_itr = group_begin; g_itr != gorup_end; ++g_itr) {
                int penalty = neighborhood->get_each_group_item_penalty(items[id], g_itr->get_id());
                if (min_penalty > penalty) {
                    min_penalty = penalty;
                    assign_group_id = g_itr->get_id();
                }
            }
            neighborhood->move({MoveItem(items[id], neighborhood->get_group_id(items[id]), assign_group_id)});
        }

        if (!best || best->get_eval_value() < neighborhood->get_eval_value()) {
            best = std::move(neighborhood);
        }
    }

    return std::move(*best);
}