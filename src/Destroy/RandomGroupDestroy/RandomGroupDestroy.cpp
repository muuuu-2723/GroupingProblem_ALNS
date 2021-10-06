#include "RandomGroupDestroy.hpp"
#include <Destroy.hpp>
#include <Solution.hpp>
#include <Item.hpp>
#include <MyRandom.hpp>
#include <Group.hpp>
#include <vector>
#include <numeric>

using std::vector;

void RandomGroupDestroy::operator()(Solution& solution) {
    vector<int> target_group_ids;
    target_group_ids.reserve(Group::N);
    auto [group_begin, group_end] = solution.get_groups_range();
    for (auto g_itr = group_begin; g_itr != group_end; ++g_itr) {
        if (g_itr->get_member_num() != 0) target_group_ids.push_back(g_itr->get_id());
    }
    MyRandom::shuffle(target_group_ids);
    vector<MoveItem> move_items;
    move_items.reserve(((Item::N / Group::N) + 1) * destroy_num);

    for (int i = 0; i < destroy_num; ++i) {
        const Group& g = solution.get_groups()[target_group_ids[i]];
        for (const auto& id : g.get_member_list()) {
            if (items[id].predefined_group != -1) continue;
            move_items.push_back(MoveItem(items[id], solution.get_group_id(items[id]), Group::N));
        }
    }

    solution.move(move_items);
}