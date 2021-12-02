#include "RelationGreedy.hpp"
#include <Solution.hpp>
#include <Group.hpp>
#include <Destroy.hpp>
#include <Item.hpp>
#include <MyRandom.hpp>
#include <vector>
#include <cfloat>
#include <memory>
#include <numeric>

using std::vector;

/*
 *�×~�@�ŐV���ȉ��𐶐�
 *destroy_ptr�ŉ���j��, ���ꂼ��̃A�C�e���Ԃ̊֌W�l�̕��ςƃA�C�e���ƃO���[�v�̊Ԃ̊֌W�l�̍��v�������O���[�v�Ɋ��蓖�Ă�
 */
std::unique_ptr<Solution> RelationGreedy::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    std::unique_ptr<Solution> best;                                                 //�����������ň�ԗǂ��]���l�̉�
    //std::cout << "rg_test" << std::endl;
    //std::cout << current_solution << std::endl;
    for (size_t i = 0; i < /*5*/10; ++i) {
        //���݂̉����R�s�[��, �����j��
        auto neighborhood = std::make_unique<Solution>(current_solution);
        auto destroy_items = (*destroy_ptr)(*neighborhood);
        vector<MoveItem> destroy_move;
        destroy_move.reserve(destroy_items.size());
        for (auto&& item : destroy_items) {
            destroy_move.push_back(MoveItem(*item, neighborhood->get_group_id(*item), neighborhood->get_dummy_group().get_id()));
        }
        neighborhood->move(destroy_move);

        //�j�󂳂ꂽ�A�C�e��(�_�~�[�O���[�v)�̏��Ԃ��V���b�t��
        const auto& member_list = neighborhood->get_dummy_group().get_member_list();
        vector<int> target_ids(member_list.begin(), member_list.end());
        MyRandom::shuffle(target_ids);
        auto [group_begin, group_end] = neighborhood->get_groups_range();
        vector<size_t> shuffle_group_ids(std::distance(group_begin, group_end));
        std::iota(shuffle_group_ids.begin(), shuffle_group_ids.end(), 0);
        MyRandom::shuffle(shuffle_group_ids);

        //�j�󂳂ꂽ�A�C�e�����֌W�l�������O���[�v�Ɋ��蓖�Ă�
        for (const auto& id : target_ids) {
            int assign_group_id = -1;
            double max_value = -DBL_MAX;
            for (auto&& g_id : shuffle_group_ids) {
                auto g_itr = group_begin + g_id;
                int group_member_num = g_itr->get_member_num();
                //item_relation�̓A�C�e�����𑽂�����Α傫���Ȃ邽�ߕ��ϒl�ŕ]��
                double value = 0;
                if (neighborhood->get_eval_flags().test(Solution::EvalIdx::ITEM_R)) {
                    value += neighborhood->get_each_group_item_relation(items[id], g_itr->get_id());
                    if (group_member_num != 0) value /= group_member_num;
                }

                if (neighborhood->get_eval_flags().test(Solution::EvalIdx::GROUP_R)) {
                    value += items[id].group_relations[g_itr->get_id()];
                }

                if (value > max_value) {
                    max_value = value;
                    assign_group_id = g_itr->get_id();
                }
            }
            //���łɊ��蓖�Ă����肵���A�C�e���Ƃ̊֌W�l���l�����邽�߂Ɉ�l�����蓖�Ă�
            neighborhood->move({MoveItem(items[id], neighborhood->get_group_id(items[id]), assign_group_id)});
            //std::cerr << "re" << std::endl;
            //std::cerr << *neighborhood << std::endl;
        }

        if (!best || best->get_eval_value() < neighborhood->get_eval_value()) {
            best = std::move(neighborhood);
            is_move = (target_ids.size() > 0);
        }
    }
    //std::cout << *best << std::endl;

    return std::move(best);
}