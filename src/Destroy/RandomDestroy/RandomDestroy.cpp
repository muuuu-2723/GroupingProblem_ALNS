#include "RandomDestroy.hpp"
#include <Destroy.hpp>
#include <Solution.hpp>
#include <Item.hpp>
#include <Group.hpp>
#include <MyRandom.hpp>
#include <vector>

using std::vector;

/*コンストラクタ*/
RandomDestroy::RandomDestroy(const std::vector<Item>& items, int destroy_num, double init_weight, int param) : Destroy(items, init_weight, param), destroy_num(destroy_num) {
    target_item_ids.reserve(Item::N);
    for (auto&& item : items) {
        if (item.predefined_group == -1) {
            target_item_ids.push_back(item.id);
        }
    }
}

/*
 *破壊法を実行
 *グループが固定されているアイテム以外のアイテムからランダムにdestroy_numだけ
 *現在のグループから除去する
 *除去されたアイテムはgroup_id = Group::Nのダミーグループに割り当てる
 */
void RandomDestroy::operator()(Solution& solution) {
    //std::cout << solution << std::endl;
    MyRandom::shuffle(target_item_ids);
    vector<MoveItem> move_items;
    move_items.reserve(destroy_num);
    //std::cerr << "random" << std::endl;
    for (size_t i = 0; i < destroy_num; ++i) {
        const Item& item = items[target_item_ids[i]];
        move_items.push_back(MoveItem(item, solution.get_group_id(item), Group::N));
    }
    //std::cerr << "random" << std::endl;
    solution.move(move_items);
    //std::cout << solution << std::endl;
}