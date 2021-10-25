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
 *�j��@�����s
 *�֌W�l�̘a����y�i���e�B�̘a���������l���Ⴂ�A�C�e���𒲍���, 
 *destroy_num���̃A�C�e������������
 *�������ꂽ�A�C�e����group_id = Group::N�̃_�~�[�O���[�v�Ɋ��蓖�Ă�
 */
void MinimumDestroy::operator()(Solution& solution) {
    std::cerr << solution << std::endl;
    std::cerr << "minimum" << std::endl;
    //�֌W�l�̘a����y�i���e�B�̘a���������l�ƃA�C�e���̃y�A�����, �\�[�g����
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