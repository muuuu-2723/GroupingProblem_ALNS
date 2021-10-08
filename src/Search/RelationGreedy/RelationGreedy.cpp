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
 *貪欲法で新たな解を生成
 *destroy_ptrで解を破壊し, それぞれのアイテム間の関係値の平均とアイテムとグループの間の関係値の合計が高いグループに割り当てる
 */
Solution RelationGreedy::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    std::unique_ptr<Solution> best;                                                 //生成した解で一番良い評価値の解
    for (size_t i = 0; i < /*40*/5; ++i) {
        //現在の解をコピーし, それを破壊
        std::unique_ptr<Solution> neighborhood(new Solution(current_solution));
        (*destroy_ptr)(*neighborhood);

        //破壊されたアイテム(ダミーグループ)の順番をシャッフル
        const auto& member_list = neighborhood->get_dummy_group().get_member_list();
        vector<int> target_ids(member_list.begin(), member_list.end());
        MyRandom::shuffle(target_ids);

        //破壊されたアイテムを関係値が高いグループに割り当てる
        for (const auto& id : target_ids) {
            int assign_group_id;
            double max_value = -DBL_MAX;
            auto [group_begin, group_end] = neighborhood->get_groups_range();
            for (auto g_itr = group_begin; g_itr != group_end; ++g_itr) {
                int group_member_num = g_itr->get_member_num();
                //item_relationはアイテム数を多くすれば大きくなるため平均値で評価
                auto& item_relations = neighborhood->get_each_group_item_relation(items[id], g_itr->get_id());
                double value = std::accumulate(item_relations.begin(), item_relations.end(), 0.0);
                value /= group_member_num;

                value += std::accumulate(items[id].group_relations[g_itr->get_id()].begin(), items[id].group_relations[g_itr->get_id()].end(), 0.0);
                if (value > max_value) {
                    max_value = value;
                    assign_group_id = g_itr->get_id();
                }
            }
            //すでに割り当てが決定したアイテムとの関係値を考慮するために一人ずつ割り当てる
            neighborhood->move({MoveItem(items[id], neighborhood->get_group_id(items[id]), assign_group_id)});
        }

        if (!best || best->get_eval_value() < neighborhood->get_eval_value()) {
            best = std::move(neighborhood);
        }
    }

    return std::move(*best);
}