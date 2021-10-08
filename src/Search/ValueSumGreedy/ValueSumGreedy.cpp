#include "ValueSumGreedy.hpp"
#include <Destroy.hpp>
#include <Solution.hpp>
#include <Item.hpp>
#include <MyRandom.hpp>
#include <vector>
#include <memory>
#include <cfloat>

using std::vector;

/*
 *貪欲法で新たな解を生成
 *destroy_ptrで解を破壊し, 追加したときにグループの合計がアイテム全体の合計をグループ数で割った値により近いグループに割り当てる
 */
Solution ValueSumGreedy::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    std::unique_ptr<Solution> best;                                                     //生成した解で一番良い評価値の解
    for (size_t i = 0; i < 40; ++i) {
        //現在の解をコピーし, それを破壊
        std::unique_ptr<Solution> neighborhood(new Solution(current_solution));
        (*destroy_ptr)(*neighborhood);

        //破壊されたアイテム(ダミーグループ)の順番をシャッフル
        auto& member_list = neighborhood->get_dummy_group().get_member_list();
        vector<int> target_ids(member_list.begin(), member_list.end());
        MyRandom::shuffle(target_ids);

        //破壊されたアイテムを合計が全体の合計をグループ数で割った値に近くなるグループに割り当てる
        for (const auto& id : target_ids) {
            int assign_group_id;
            double min_value = DBL_MAX;
            auto [group_begin, group_end] = neighborhood->get_groups_range();

            for (auto g_itr = group_begin; g_itr != group_end; ++g_itr) {
                double value = 0;
                for (size_t j = 0; j < Item::v_size; ++j) {
                    double new_sum = g_itr->get_sum_values()[j] + items[id].values[j];
                    value += std::abs(new_sum - neighborhood->get_sum_values()[j]);
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

    return std::move(*best);
}