#include "ItemPenaltyGreedy.hpp"
#include <Solution.hpp>
#include <Destroy.hpp>
#include <Item.hpp>
#include <MyRandom.hpp>
#include <vector>
#include <memory>
#include <climits>
#include <numeric>

using std::vector;

ItemPenaltyGreedy::ItemPenaltyGreedy(const std::vector<Item>& items, double init_weight, int param, const Solution& solution) : Search(items, init_weight, param, solution) {
    destructions.emplace_back(std::make_shared<RandomDestroy>(items, init_item_destroy_num, 1, 1, solution));
    destructions.emplace_back(std::make_shared<RandomGroupDestroy>(items, init_group_destroy_num, 1, 1, solution));
    destructions.emplace_back(std::make_shared<MinimumDestroy>(items, init_item_destroy_num, 1, 1, solution));
    destructions.emplace_back(std::make_shared<MinimumGroupDestroy>(items, init_group_destroy_num, 1, 1, solution));
    destructions.emplace_back(std::make_shared<UpperWeightGreedyDestroy>(items, init_group_destroy_num, 1, 1, solution));
    
    init_destroy_random();

    for (auto&& d : destructions) {
        if (typeid(*d) == typeid(RandomDestroy) || typeid(*d) == typeid(MinimumDestroy)) {
            item_destroy.emplace_back(d);
        }
        else {
            group_destroy.emplace_back(d);
        }
    }
}

/*
 *貪欲法で新たな解を生成
 *destroy_ptrで解を破壊し, それぞれのアイテム間のペナルティの合計が少ないグループに割り当てる
 */
std::unique_ptr<Solution> ItemPenaltyGreedy::operator()(const Solution& current_solution) {
    std::unique_ptr<Solution> best;                                                             //生成した解で一番良い評価値の解
    //std::cout << "ip_test" << std::endl;
    //std::cout << current_solution << std::endl;
    auto& destroy = select_destroy();
    for (size_t i = 0; i < /*40*/10; ++i) {
        //現在の解をコピーし, それを破壊
        auto neighborhood = std::make_unique<Solution>(current_solution);
        auto destroy_items = destroy(*neighborhood);
        vector<MoveItem> destroy_move;
        destroy_move.reserve(destroy_items.size());
        for (auto&& item : destroy_items) {
            destroy_move.push_back(MoveItem(*item, neighborhood->get_group_id(*item), neighborhood->get_dummy_group().get_id()));
        }
        neighborhood->move(destroy_move);

        //破壊されたアイテム(ダミーグループ)の順番をシャッフル
        auto& member_list = neighborhood->get_dummy_group().get_member_list();
        vector<int> target_ids(member_list.begin(), member_list.end());
        MyRandom::shuffle(target_ids);
        auto [group_begin, group_end] = neighborhood->get_groups_range();
        vector<size_t> shuffle_group_ids(std::distance(group_begin, group_end));
        std::iota(shuffle_group_ids.begin(), shuffle_group_ids.end(), 0);
        MyRandom::shuffle(shuffle_group_ids);

        //破壊されたアイテムをitem_penaltyの合計が最も少ないグループに割り当て
        for (const auto& id : target_ids) {
            int assign_group_id;
            int min_penalty = INT_MAX;
            for (auto&& g_id : shuffle_group_ids) {
                auto g_itr = group_begin + g_id;
                int penalty = 0;
                if (neighborhood->get_eval_flags().test(Solution::EvalIdx::ITEM_PENA)) {
                    penalty = neighborhood->get_each_group_item_penalty(items[id], g_itr->get_id());
                }
                if (min_penalty > penalty) {
                    min_penalty = penalty;
                    assign_group_id = g_itr->get_id();
                }
            }
            //すでに割り当てが決定したアイテムとのペナルティを考慮するために一人ずつ割り当てる
            neighborhood->move({MoveItem(items[id], neighborhood->get_group_id(items[id]), assign_group_id)});
        }

        if (!best || best->get_eval_value() < neighborhood->get_eval_value()) {
            best = std::move(neighborhood);
            is_move = (target_ids.size() > 0);
        }
    }
    //std::cout << *best << std::endl;
    return std::move(best);
}