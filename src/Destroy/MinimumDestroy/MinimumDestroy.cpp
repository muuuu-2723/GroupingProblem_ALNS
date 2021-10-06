#include "MinimumDestroy.hpp"
#include <Destroy.hpp>
#include <Solution.hpp>
#include <Group.hpp>
#include <Item.hpp>
#include <vector>
#include <tuple>
#include <algorithm>

using std::vector;

void MinimumDestroy::operator()(Solution& solution) {
    vector<std::pair<double, const Item&>> item_eval;
    item_eval.reserve(Item::N);
    for (const auto& item : items) {
        if (item.predefined_group == -1) {
            const Group& g = solution.get_groups()[solution.get_group_id(item)];
            double eval = 0;
            for (auto&& r : solution.get_each_group_item_relation(item, g.get_id())) {
                eval += r * solution.get_relation_parameter();
            }
            for (auto&& r : item.group_relations[g.get_id()]) {
                eval += r * solution.get_relation_parameter();
            }
            eval += g.diff_weight_penalty({}, {&item}) * solution.get_penalty_parameter();
            eval -= item.group_penalty[g.get_id()] * solution.get_penalty_parameter();
            eval -= solution.get_each_group_item_penalty(item, g.get_id()) * solution.get_penalty_parameter();
            item_eval.push_back({eval, item});
        }
    }

    std::sort(item_eval.begin(), item_eval.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

    vector<MoveItem> move_items;
    move_items.reserve(destroy_num);
    for (size_t i = 0; i < destroy_num; ++i) {
        move_items.push_back(MoveItem(item_eval[i].second, solution.get_group_id(item_eval[i].second), Group::N));
    }

    solution.move(move_items);
}