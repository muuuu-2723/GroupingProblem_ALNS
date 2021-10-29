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
 *各グループのvalueの平均を平滑化する貪欲法
 *destroy_ptrで解を破壊し, 追加したときにグループの平均がアイテム全体の平均により近いグループに割り当てる
 */
std::unique_ptr<Solution> ValueAverageGreedy::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    std::unique_ptr<Solution> best;                                                     //生成した解で一番良い評価値の解
    std::cout << "va_test" << std::endl;
    //std::cout << current_solution << std::endl;
    for (size_t i = 0; i < 40; ++i) {
        //現在の解をコピーし, それを破壊
        auto neighborhood = std::make_unique<Solution>(current_solution);
        (*destroy_ptr)(*neighborhood);

        //破壊されたアイテム(ダミーグループ)の順番をシャッフル
        auto& member_list = neighborhood->get_dummy_group().get_member_list();
        vector<int> target_ids(member_list.begin(), member_list.end());
        MyRandom::shuffle(target_ids);

        //破壊されたアイテムを平均値が全体の平均値に近くなるグループに割り当てる
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
            //すでに割り当てが決定したアイテムのvalueを考慮するために一人ずつ割り当てる
            neighborhood->move({MoveItem(items[id], neighborhood->get_group_id(items[id]), assign_group_id)});
        }

        if (!best || best->get_eval_value() < neighborhood->get_eval_value()) {
            best = std::move(neighborhood);
        }
    }
    //std::cout << *best << std::endl;

    return std::move(best);
}