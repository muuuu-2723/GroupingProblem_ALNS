#include "MinimumGroupDestroy.hpp"
#include <Destroy.hpp>
#include <Solution.hpp>
#include <Group.hpp>
#include <Item.hpp>
#include <vector>
#include <tuple>
#include <algorithm>

using std::vector;

/*
 *破壊法を実行
 *関係値の和からペナルティの和を引いた値の和が低いグループを調査し, 
 *destroy_numグループを選び, そのグループに所属するアイテムを除去する
 *(グループが固定しているアイテムを除く)
 *除去されたアイテムはgroup_id = Group::Nのダミーグループに割り当てる
 */
void MinimumGroupDestroy::operator()(Solution& solution) {
    //関係値の和からペナルティの和を引いた値の和とグループのペアを作り, ソートする
    vector<std::pair<double, const Group&>> group_eval;
    group_eval.reserve(Group::N);
    for (auto&& group : solution.get_valid_groups()) {
        double eval = 0;
        vector<const Item*> target_member;
        target_member.reserve(group->get_member_num());
        for (const auto& id : group->get_member_list()) {
            if (items[id].predefined_group != -1) continue;
            for (auto&& r : solution.get_each_group_item_relation(items[id], group->get_id())) {
                eval += r * solution.get_relation_parameter();
            }
            for (auto&& r : items[id].group_relations[group->get_id()]) {
                eval += r * solution.get_relation_parameter();
            }
            eval -= solution.get_each_group_item_penalty(items[id], group->get_id()) * solution.get_penalty_parameter();
            eval -= items[id].group_penalty[group->get_id()] * solution.get_penalty_parameter();

            target_member.push_back(&items[id]);
        }
        eval += group->diff_weight_penalty({}, target_member) * solution.get_penalty_parameter();
        for (auto m_itr1 = target_member.begin(), end = target_member.end(); m_itr1 != end; ++m_itr1) {
            for (auto m_itr2 = std::next(m_itr1); m_itr2 != end; ++m_itr2) {
                eval += (*m_itr1)->item_penalty[(*m_itr2)->id] * solution.get_penalty_parameter();
                for (size_t i = 0; i < Item::item_r_size; ++i) {
                    eval -= (*m_itr1)->item_relations[(*m_itr2)->id][i];
                }
            }
        }

        group_eval.push_back({eval, *group});
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