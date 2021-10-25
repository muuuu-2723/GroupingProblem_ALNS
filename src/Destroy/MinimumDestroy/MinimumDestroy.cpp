#include "MinimumDestroy.hpp"
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
 *関係値の和からペナルティの和を引いた値が低いアイテムを調査し, 
 *destroy_num分のアイテムを除去する
 *除去されたアイテムはgroup_id = Group::Nのダミーグループに割り当てる
 */
void MinimumDestroy::operator()(Solution& solution) {
    std::cerr << solution << std::endl;
    std::cerr << "minimum" << std::endl;
    //関係値の和からペナルティの和を引いた値とアイテムのペアを作り, ソートする
    vector<std::pair<double, size_t>> item_eval;
    item_eval.reserve(Item::N);
    for (const auto& item : items) {
        if (item.predefined_group == -1) {
            //std::cerr << item.id << std::endl;
            double eval = solution.calc_diff_eval(solution.evaluation_diff({MoveItem(item, solution.get_group_id(item), Group::N)}));
            item_eval.push_back({eval, item.id});
        }
    }
    std::cerr << "minimum" << std::endl;

    std::sort(item_eval.begin(), item_eval.end(), [](const auto& a, const auto& b) { return a.first > b.first; });

    vector<MoveItem> move_items;
    move_items.reserve(destroy_num);
    for (size_t i = 0; i < destroy_num; ++i) {
        const Item& item = items[item_eval[i].second];
        move_items.push_back(MoveItem(item, solution.get_group_id(item), Group::N));
    }
    std::cerr << "minimum" << std::endl;
    solution.move(move_items);
}