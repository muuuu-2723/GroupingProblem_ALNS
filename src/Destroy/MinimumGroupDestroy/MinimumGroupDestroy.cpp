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
 *�j��@�����s
 *�֌W�l�̘a����y�i���e�B�̘a���������l�̘a���Ⴂ�O���[�v�𒲍���, 
 *destroy_num�O���[�v��I��, ���̃O���[�v�ɏ�������A�C�e������������
 *(�O���[�v���Œ肵�Ă���A�C�e��������)
 *�������ꂽ�A�C�e����group_id = Group::N�̃_�~�[�O���[�v�Ɋ��蓖�Ă�
 */
void MinimumGroupDestroy::operator()(Solution& solution) {
    //�֌W�l�̘a����y�i���e�B�̘a���������l�̘a�ƃO���[�v�̃y�A�����, �\�[�g����
    vector<std::pair<double, const Group&>> group_eval;
    group_eval.reserve(Group::N);
    for (auto&& group : solution.get_valid_groups()) {
        vector<MoveItem> target_items;
        target_items.reserve(group->get_member_num());
        for (auto&& id : group->get_member_list()) {
            if (items[id].predefined_group != -1) continue;
            target_items.push_back(MoveItem(items[id], group->get_id(), Group::N));
        }

        double eval = solution.calc_diff_eval(solution.evaluation_diff(target_items));
        group_eval.push_back({eval, *group});
    }

    std::sort(group_eval.begin(), group_eval.end(), [](const auto& a, const auto& b) { return a.first > b.first; });

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