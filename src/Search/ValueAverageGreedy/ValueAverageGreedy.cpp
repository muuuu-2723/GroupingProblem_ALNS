#include "ValueAverageGreedy.hpp"
#include <Destroy.hpp>
#include <Solution.hpp>
#include <Item.hpp>
#include <MyRandom.hpp>
#include <vector>
#include <memory>
#include <cfloat>

using std::vector;

/*
 *�e�O���[�v��value�̕��ς𕽊��������×~�@
 *destroy_ptr�ŉ���j��, �ǉ������Ƃ��ɃO���[�v�̕��ς��A�C�e���S�̂̕��ςɂ��߂��O���[�v�Ɋ��蓖�Ă�
 */
std::unique_ptr<Solution> ValueAverageGreedy::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    std::unique_ptr<Solution> best;                                                     //�����������ň�ԗǂ��]���l�̉�
    std::cout << "va_test" << std::endl;
    //std::cout << current_solution << std::endl;
    for (size_t i = 0; i < 40; ++i) {
        //���݂̉����R�s�[��, �����j��
        auto neighborhood = std::make_unique<Solution>(current_solution);
        (*destroy_ptr)(*neighborhood);

        //�j�󂳂ꂽ�A�C�e��(�_�~�[�O���[�v)�̏��Ԃ��V���b�t��
        auto& member_list = neighborhood->get_dummy_group().get_member_list();
        vector<int> target_ids(member_list.begin(), member_list.end());
        MyRandom::shuffle(target_ids);

        //�j�󂳂ꂽ�A�C�e���𕽋ϒl���S�̂̕��ϒl�ɋ߂��Ȃ�O���[�v�Ɋ��蓖�Ă�
        for (const auto& id : target_ids) {
            int assign_group_id;
            double min_value = DBL_MAX;
            auto [group_begin, group_end] = neighborhood->get_groups_range();
            
            for (auto g_itr = group_begin; g_itr != group_end; ++g_itr) {
                double value = 0;
                for (size_t j = 0; j < Item::v_size; ++j) {
                    double new_ave = (g_itr->get_sum_values()[j] + items[id].values[j]) / (g_itr->get_member_num() + 1);
                    value += std::abs(new_ave - neighborhood->get_ave()[j]);
                }
                if (min_value > value) {
                    min_value = value;
                    assign_group_id = g_itr->get_id();
                }
            }
            //���łɊ��蓖�Ă����肵���A�C�e����value���l�����邽�߂Ɉ�l�����蓖�Ă�
            neighborhood->move({MoveItem(items[id], neighborhood->get_group_id(items[id]), assign_group_id)});
        }

        if (!best || best->get_eval_value() < neighborhood->get_eval_value()) {
            best = std::move(neighborhood);
        }
    }
    //std::cout << *best << std::endl;

    return std::move(best);
}