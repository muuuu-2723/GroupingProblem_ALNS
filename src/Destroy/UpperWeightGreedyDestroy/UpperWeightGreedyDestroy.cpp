#include "UpperWeightGreedyDestroy.hpp"
#include <Destroy.hpp>
#include <Solution.hpp>
#include <Item.hpp>
#include <Group.hpp>
#include <vector>
#include <tuple>
#include <algorithm>

using std::vector;

void UpperWeightGreedyDestroy::operator()(Solution& solution) {
    vector<std::pair<double, size_t>> group_eval;
    group_eval.reserve(Group::N);
    for (auto&& group : solution.get_valid_groups()) {
        double eval = 0;
        for (size_t i = 0; i < Item::w_size; ++i) {
            eval += group->get_upper()[i] - group->get_sum_weight()[i];
        }
        group_eval.push_back({eval, group->get_id()});
    }

    std::sort(group_eval.begin(), group_eval.end(), [](const auto& a, const auto& b) { return a.first > b.first; });

    vector<MoveItem> move_items;
    size_t actual_destroy_num = destroy_num;
    if (solution.get_eval_flags().test(Solution::EvalIdx::GROUP_NUM)) {
        actual_destroy_num = std::min(destroy_num, (int)solution.get_valid_groups().size());
    }
    for (size_t i = 0; i < actual_destroy_num; ++i) {
        auto& g = solution.get_groups()[group_eval[i].second];
        for (auto&& id : g.get_member_list()) {
            if (items[id].predefined_group != -1) continue;
            move_items.push_back(MoveItem(items[id], g.get_id(), Group::N));
        }
    }

    solution.move(move_items);
}