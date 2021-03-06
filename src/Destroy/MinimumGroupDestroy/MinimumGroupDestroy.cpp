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
vector<const Item*> MinimumGroupDestroy::operator()(Solution& solution) const {
    //std::cout << solution << std::endl;
    //std::cerr << "minimum_group" << std::endl;
    //関係値の和からペナルティの和を引いた値の和とグループのペアを作り, ソートする
    vector<std::pair<double, size_t>> group_eval;
    group_eval.reserve(Group::N);
    for (auto&& group : solution.get_valid_groups()) {
        vector<MoveItem> target_items;
        target_items.reserve(group->get_member_num());
        for (auto&& id : group->get_member_list()) {
            if (items[id].predefined_group != -1) continue;
            //std::cerr << id << std::endl;
            target_items.push_back(MoveItem(items[id], group->get_id(), Group::N));
        }

        double eval = solution.calc_diff_eval(solution.evaluation_diff(target_items));
        group_eval.push_back({eval, group->get_id()});
    }

    std::sort(group_eval.begin(), group_eval.end(), [](const auto& a, const auto& b) { return a.first > b.first; });

    size_t actual_destroy_num = destroy_num;
    if (solution.get_eval_flags().test(Solution::EvalIdx::GROUP_COST)) {
        actual_destroy_num = std::min(destroy_num, (int)solution.get_valid_groups().size());
    }

    vector<const Item*> move_items;
    move_items.reserve(Item::N);
    for (size_t i = 0; i < actual_destroy_num; ++i) {
        auto& g = solution.get_groups()[group_eval[i].second];
        for (auto&& id : g.get_member_list()) {
            if (items[id].predefined_group != -1) continue;
            move_items.push_back(&items[id]);
        }
    }
    return move_items;
    
    /*vector<MoveItem> move_items;
    for (size_t i = 0; i < actual_destroy_num; ++i) {
        const Group& g = solution.get_groups()[group_eval[i].second];
        for (const auto& id : g.get_member_list()) {
            if (items[id].predefined_group != -1) continue;
            move_items.push_back(MoveItem(items[id], g.get_id(), Group::N));
        }
    }

    solution.move(move_items);*/
    //std::cout << solution << std::endl;
}