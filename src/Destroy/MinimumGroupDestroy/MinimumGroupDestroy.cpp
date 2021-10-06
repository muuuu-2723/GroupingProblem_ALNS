#include "MinimumGroupDestroy.hpp"
#include <Destroy.hpp>
#include <Solution.hpp>
#include <Group.hpp>
#include <Item.hpp>
#include <vector>
#include <tuple>
#include <algorithm>

using std::vector;

void MinimumGroupDestroy::operator()(Solution& solution) {
    vector<std::pair<double, const Group&>> group_eval;
    group_eval.reserve(Group::N);
    auto [group_begin, group_end] = solution.get_groups_range();
    for (auto g_itr = group_begin; g_itr != group_end; ++g_itr) {
        double eval = 0;
        vector<const Item*> target_member;
        target_member.reserve(g_itr->get_member_num());
        for (const auto& id : g_itr->get_member_list()) {
            if (items[id].predefined_group != -1) continue;
            for (auto&& r : solution.get_each_group_item_relation(items[id], g_itr->get_id())) {
                eval += r * solution.get_relation_parameter();
            }
            for (auto&& r : items[id].group_relations[g_itr->get_id()]) {
                eval += r * solution.get_relation_parameter();
            }
            eval -= solution.get_each_group_item_penalty(items[id], g_itr->get_id()) * solution.get_penalty_parameter();
            eval -= items[id].group_penalty[g_itr->get_id()] * solution.get_penalty_parameter();

            target_member.push_back(&items[id]);
        }
        eval += g_itr->diff_weight_penalty({}, target_member) * solution.get_penalty_parameter();
        for (auto m_itr1 = target_member.begin(), end = target_member.end(); m_itr1 != end; ++m_itr1) {
            for (auto m_itr2 = std::next(m_itr1); m_itr2 != end; ++m_itr2) {
                eval += (*m_itr1)->item_penalty[(*m_itr2)->id] * solution.get_penalty_parameter();
                for (size_t i = 0; i < Item::item_r_size; ++i) {
                    eval -= (*m_itr1)->item_relations[(*m_itr2)->id][i];
                }
            }
        }

        group_eval.push_back({eval, *g_itr});
    }

    std::sort(group_eval.begin(), group_eval.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

    vector<MoveItem> move_items;
    for (size_t i = 0; i < destroy_num; ++i) {
        const Group& g = group_eval[i].second;
        for (const auto& id : g.get_member_list()) {
            if (items[id].predefined_group != -1) continue;
            move_items.push_back(MoveItem(items[id], g.get_id(), Group::N));
        }
    }

    solution.move(move_items);
}