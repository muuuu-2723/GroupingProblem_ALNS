#include "DecreaseGroup.hpp"
#include <Solution.hpp>
#include <Destroy.hpp>
#include <Group.hpp>
#include <Item.hpp>
#include <vector>
#include <memory>
#include <climits>

using std::vector;

Solution DecreaseGroup::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    std::unique_ptr<Solution> best;
    for (size_t i = 0; i < 40; ++i) {
        std::unique_ptr<Solution> neighborhood(new Solution(current_solution));
        (*destroy_ptr)(*neighborhood);

        auto& dummy_group = neighborhood->get_dummy_group();
        vector<MoveItem> move_items;
        move_items.reserve(dummy_group.get_member_num());
        vector<vector<const Item*>> add_members(Group::N);
        for (auto&& id : dummy_group.get_member_list()) {
            int assign_group_id;
            int min_penalty = INT_MAX;
            for (auto&& group : neighborhood->get_valid_groups()) {
                vector<const Item*> tmp = add_members[group->get_id()];
                tmp.push_back(&items[id]);
                int diff_penalty = 0;
                if (neighborhood->get_eval_flags().test(Solution::EvalIdx::WEIGHT_PENA)) {
                    diff_penalty += group->diff_weight_penalty(tmp, {}) - group->diff_weight_penalty(add_members[group->get_id()], {});
                }
                if (diff_penalty < min_penalty) {
                    min_penalty = diff_penalty;
                    assign_group_id = group->get_id();
                }
            }
            move_items.push_back(MoveItem(items[id], neighborhood->get_group_id(items[id]), assign_group_id));
            add_members[assign_group_id].push_back(&items[id]);
        }
        neighborhood->move(move_items);

        if (!best || best->get_eval_value() < neighborhood->get_eval_value()) {
            best = std::move(neighborhood);
        }
    }

    return std::move(*best);
}