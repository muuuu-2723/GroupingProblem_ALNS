#include "RandomDestroy.hpp"
#include <Destroy.hpp>
#include <Solution.hpp>
#include <Item.hpp>
#include <Group.hpp>
#include <MyRandom.hpp>
#include <vector>

using std::vector;

RandomDestroy::RandomDestroy(std::vector<Item>& items, int destroy_num, double init_weight, int param) : Destroy(items, init_weight, param), destroy_num(destroy_num) {
    target_item_ids.reserve(Item::N);
    for (auto&& item : items) {
        if (item.predefined_group == -1) {
            target_item_ids.push_back(item.id);
        }
    }
}

void RandomDestroy::operator()(Solution& solution) {
    MyRandom::shuffle(target_item_ids);
    vector<MoveItem> move_items;
    move_items.reserve(destroy_num);

    for (size_t i = 0; i < destroy_num; ++i) {
        const Item& item = items[target_item_ids[i]];
        move_items.push_back(MoveItem(item, solution.get_group_id(item), Group::N));
    }
    solution.move(move_items);
}