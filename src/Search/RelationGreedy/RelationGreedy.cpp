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
std::unique_ptr<Solution> RelationGreedy::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    std::unique_ptr<Solution> best;                                                 //生成した解で一番良い評価値の解
    //std::cout << "rg_test" << std::endl;
    //std::cout << current_solution << std::endl;
    for (size_t i = 0; i < /*5*/10; ++i) {
        //現在の解をコピーし, それを破壊
        auto neighborhood = std::make_unique<Solution>(current_solution);
        auto destroy_items = (*destroy_ptr)(*neighborhood);
        vector<MoveItem> destroy_move;
        destroy_move.reserve(destroy_items.size());
        for (auto&& item : destroy_items) {
            destroy_move.push_back(MoveItem(*item, neighborhood->get_group_id(*item), neighborhood->get_dummy_group().get_id()));
        }
        neighborhood->move(destroy_move);

        //破壊されたアイテム(ダミーグループ)の順番をシャッフル
        const auto& member_list = neighborhood->get_dummy_group().get_member_list();
        vector<int> target_ids(member_list.begin(), member_list.end());
        MyRandom::shuffle(target_ids);
        auto [group_begin, group_end] = neighborhood->get_groups_range();
        vector<size_t> shuffle_group_ids(std::distance(group_begin, group_end));
        std::iota(shuffle_group_ids.begin(), shuffle_group_ids.end(), 0);
        MyRandom::shuffle(shuffle_group_ids);

        //破壊されたアイテムを関係値が高いグループに割り当てる
        for (const auto& id : target_ids) {
            int assign_group_id = -1;
            double max_value = -DBL_MAX;
            for (auto&& g_id : shuffle_group_ids) {
                auto g_itr = group_begin + g_id;
                int group_member_num = g_itr->get_member_num();
                //item_relationはアイテム数を多くすれば大きくなるため平均値で評価
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
            //すでに割り当てが決定したアイテムとの関係値を考慮するために一人ずつ割り当てる
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