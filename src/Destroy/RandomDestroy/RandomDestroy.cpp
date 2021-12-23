#include "RandomDestroy.hpp"
#include <Destroy.hpp>
#include <Solution.hpp>
#include <Item.hpp>
#include <Group.hpp>
#include <MyRandom.hpp>
#include <vector>

using std::vector;

/*�R���X�g���N�^*/
RandomDestroy::RandomDestroy(const std::vector<Item>& items, int destroy_num, double init_weight, int param, const Solution& solution, double upper_destroy_ratio) : Destroy(items, init_weight, param) {
    this->upper_destroy_ratio = upper_destroy_ratio;
    set_destroy_num(destroy_num, solution);
    target_items.reserve(Item::N);
    for (auto&& item : items) {
        if (item.predefined_group == -1) {
            target_items.push_back(&item);
        }
    }
}

/*
 *�j��@�����s
 *�O���[�v���Œ肳��Ă���A�C�e���ȊO�̃A�C�e�����烉���_����destroy_num����
 *���݂̃O���[�v���珜������
 *�������ꂽ�A�C�e����group_id = Group::N�̃_�~�[�O���[�v�Ɋ��蓖�Ă�
 */
vector<const Item*> RandomDestroy::operator()(Solution& solution) const {
    //std::cout << solution << std::endl;
    vector<const Item*> move_items;
    move_items.reserve(destroy_num);
    MyRandom::sample(target_items, move_items, destroy_num);
    return move_items;
    /*MyRandom::shuffle(target_item_ids);
    vector<MoveItem> move_items;
    move_items.reserve(destroy_num);

    for (size_t i = 0; i < destroy_num; ++i) {
        const Item& item = items[target_item_ids[i]];
        move_items.push_back(MoveItem(item, solution.get_group_id(item), Group::N));
    }
    
    solution.move(move_items);*/
    //std::cout << solution << std::endl;
}