#include "ItemPenaltyGreedy.hpp"
#include <Solution.hpp>
#include <Destroy.hpp>
#include <Item.hpp>
#include <MyRandom.hpp>
#include <vector>
#include <memory>
#include <climits>

using std::vector;

/*
 *�×~�@�ŐV���ȉ��𐶐�
 *destroy_ptr�ŉ���j��, ���ꂼ��̃A�C�e���Ԃ̃y�i���e�B�̍��v�����Ȃ��O���[�v�Ɋ��蓖�Ă�
 */
std::unique_ptr<Solution> ItemPenaltyGreedy::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    std::unique_ptr<Solution> best;                                                             //�����������ň�ԗǂ��]���l�̉�
    for (size_t i = 0; i < 40; ++i) {
        //���݂̉����R�s�[��, �����j��
        auto neighborhood = std::make_unique<Solution>(current_solution);
        std::cerr << "test" << std::endl;
        (*destroy_ptr)(*neighborhood);
        std::cerr << "test" << std::endl;

        //�j�󂳂ꂽ�A�C�e��(�_�~�[�O���[�v)�̏��Ԃ��V���b�t��
        auto& member_list = neighborhood->get_dummy_group().get_member_list();
        vector<int> target_ids(member_list.begin(), member_list.end());
        MyRandom::shuffle(target_ids);

        //�j�󂳂ꂽ�A�C�e����item_penalty�̍��v���ł����Ȃ��O���[�v�Ɋ��蓖��
        for (const auto& id : target_ids) {
            int assign_group_id;
            int min_penalty = INT_MAX;
            auto [group_begin, gorup_end] = neighborhood->get_groups_range();
            for (auto g_itr = group_begin; g_itr != gorup_end; ++g_itr) {
                int penalty = 0;
                if (neighborhood->get_eval_flags().test(Solution::EvalIdx::ITEM_PENA)) {
                    penalty = neighborhood->get_each_group_item_penalty(items[id], g_itr->get_id());
                }
                if (min_penalty > penalty) {
                    min_penalty = penalty;
                    assign_group_id = g_itr->get_id();
                }
            }
            //���łɊ��蓖�Ă����肵���A�C�e���Ƃ̃y�i���e�B���l�����邽�߂Ɉ�l�����蓖�Ă�
            neighborhood->move({MoveItem(items[id], neighborhood->get_group_id(items[id]), assign_group_id)});
        }

        if (!best || best->get_eval_value() < neighborhood->get_eval_value()) {
            best = std::move(neighborhood);
        }
    }

    return std::move(best);
}