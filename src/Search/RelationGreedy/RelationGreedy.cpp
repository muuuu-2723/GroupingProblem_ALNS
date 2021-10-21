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
Solution RelationGreedy::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    std::unique_ptr<Solution> best;                                                 //�����������ň�ԗǂ��]���l�̉�
    for (size_t i = 0; i < /*40*/5; ++i) {
        //���݂̉����R�s�[��, �����j��
        std::unique_ptr<Solution> neighborhood(new Solution(current_solution));
        (*destroy_ptr)(*neighborhood);

        //�j�󂳂ꂽ�A�C�e��(�_�~�[�O���[�v)�̏��Ԃ��V���b�t��
        const auto& member_list = neighborhood->get_dummy_group().get_member_list();
        vector<int> target_ids(member_list.begin(), member_list.end());
        MyRandom::shuffle(target_ids);

        //�j�󂳂ꂽ�A�C�e�����֌W�l�������O���[�v�Ɋ��蓖�Ă�
        for (const auto& id : target_ids) {
            int assign_group_id;
            double max_value = -DBL_MAX;
            auto [group_begin, group_end] = neighborhood->get_groups_range();
            for (auto g_itr = group_begin; g_itr != group_end; ++g_itr) {
                int group_member_num = g_itr->get_member_num();
                //item_relation�̓A�C�e�����𑽂�����Α傫���Ȃ邽�ߕ��ϒl�ŕ]��
                double value = 0;
                if (neighborhood->get_eval_flags().test(Solution::EvalIdx::ITEM_R)) {
                    auto& item_relations = neighborhood->get_each_group_item_relation(items[id], g_itr->get_id());
                    value += std::accumulate(item_relations.begin(), item_relations.end(), 0.0);
                    value /= group_member_num;
                }

                if (neighborhood->get_eval_flags().test(Solution::EvalIdx::GROUP_R)) {
                    value += std::transform_reduce(items[id].group_relations[g_itr->get_id()].begin(), items[id].group_relations[g_itr->get_id()].end(),
                                                   neighborhood->get_group_relation_params().begin(), 0);
                }

                if (value > max_value) {
                    max_value = value;
                    assign_group_id = g_itr->get_id();
                }
            }
            //���łɊ��蓖�Ă����肵���A�C�e���Ƃ̊֌W�l���l�����邽�߂Ɉ�l�����蓖�Ă�
            neighborhood->move({MoveItem(items[id], neighborhood->get_group_id(items[id]), assign_group_id)});
        }

        if (!best || best->get_eval_value() < neighborhood->get_eval_value()) {
            best = std::move(neighborhood);
        }
    }

    return std::move(*best);
}