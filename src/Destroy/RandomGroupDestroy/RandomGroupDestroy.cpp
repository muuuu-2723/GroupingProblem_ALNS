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
 *�j��@�����s
 *���݂̃O���[�v����destroy_num�̃O���[�v��I��, 
 *�O���[�v���Œ肳��Ă���A�C�e�����������̃O���[�v�ɏ�������A�C�e������������
 *�������ꂽ�A�C�e����group_id = Group::N�̃_�~�[�O���[�v�Ɋ��蓖�Ă�
 */
void RandomGroupDestroy::operator()(Solution& solution) {
    //�O���[�v�ɏ�������A�C�e��������O���[�v��Ώۂɂ���
    vector<int> target_group_ids;
    target_group_ids.reserve(Group::N);
    for (auto&& group : solution.get_valid_groups()) {
        target_group_ids.push_back(group->get_id());
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