#include "RandomDestroy.hpp"
#include <Destroy.hpp>
#include <Solution.hpp>
#include <Item.hpp>
#include <Group.hpp>
#include <MyRandom.hpp>
#include <vector>

using std::vector;

/*�R���X�g���N�^*/
RandomDestroy::RandomDestroy(const std::vector<Item>& items, int destroy_num, double init_weight, int param) : Destroy(items, init_weight, param), destroy_num(destroy_num) {
    target_item_ids.reserve(Item::N);
    for (auto&& item : items) {
        if (item.predefined_group == -1) {
            target_item_ids.push_back(item.id);
        }
    }
}

/*
 *�j��@�����s
 *�O���[�v���Œ肳��Ă���A�C�e���ȊO�̃A�C�e�����烉���_����destroy_num����
 *���݂̃O���[�v���珜������
 *�������ꂽ�A�C�e����group_id = Group::N�̃_�~�[�O���[�v�Ɋ��蓖�Ă�
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