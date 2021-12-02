#include "RandomGroupDestroy.hpp"
#include <Destroy.hpp>
#include <Solution.hpp>
#include <Item.hpp>
#include <MyRandom.hpp>
#include <Group.hpp>
#include <vector>
#include <numeric>

using std::vector;

/*
 *破壊法を実行
 *現在のグループからdestroy_numのグループを選び, 
 *グループが固定されているアイテムを除くそのグループに所属するアイテムを除去する
 *除去されたアイテムはgroup_id = Group::Nのダミーグループに割り当てる
 */
vector<const Item*> RandomGroupDestroy::operator()(Solution& solution) const {
    //std::cout << solution << std::endl;
    //グループに所属するアイテムがあるグループを対象にする
    vector<const Group*> target_groups;
    target_groups.reserve(Group::N);
    for (auto&& group : solution.get_valid_groups()) {
        target_groups.push_back(group);
    }

    size_t actual_destroy_num = destroy_num;
    if (solution.get_eval_flags().test(Solution::EvalIdx::GROUP_COST)) {
        actual_destroy_num = std::min(destroy_num, (int)target_groups.size());
    }

    vector<const Group*> select_groups;
    select_groups.reserve(actual_destroy_num);
    MyRandom::sample(target_groups, select_groups, actual_destroy_num);

    vector<const Item*> move_items;
    move_items.reserve(Item::N);
    for (auto&& group : select_groups) {
        for (auto&& id : group->get_member_list()) {
            if (items[id].predefined_group != -1) continue;
            move_items.push_back(&items[id]);
        }
    }
    return move_items;
    /*vector<int> target_group_ids;
    target_group_ids.reserve(Group::N);
    for (auto&& group : solution.get_valid_groups()) {
        target_group_ids.push_back(group->get_id());
    }
    
    MyRandom::shuffle(target_group_ids);
    vector<MoveItem> move_items;
    move_items.reserve(Item::N);
    size_t actual_destroy_num = destroy_num;
    if (solution.get_eval_flags().test(Solution::EvalIdx::GROUP_COST)) {
        actual_destroy_num = std::min(destroy_num, (int)target_group_ids.size());
    }
    for (int i = 0; i < actual_destroy_num; ++i) {
        const Group& g = solution.get_groups()[target_group_ids[i]];
        for (const auto& id : g.get_member_list()) {
            if (items[id].predefined_group != -1) continue;
            move_items.push_back(MoveItem(items[id], solution.get_group_id(items[id]), Group::N));
        }
    }
    
    solution.move(move_items);*/
    //std::cout << solution << std::endl;
}