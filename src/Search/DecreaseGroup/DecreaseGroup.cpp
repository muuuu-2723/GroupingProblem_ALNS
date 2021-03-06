#include "DecreaseGroup.hpp"
#include <Solution.hpp>
#include <Destroy.hpp>
#include <Group.hpp>
#include <Item.hpp>
#include <MyRandom.hpp>
#include <vector>
#include <memory>
#include <climits>
#include <algorithm>
#include <typeinfo>

using std::vector;

DecreaseGroup::DecreaseGroup(const std::vector<Item>& items, double init_weight, int param, const Solution& solution) : Search(items, init_weight, param, solution) {
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

std::unique_ptr<Solution> DecreaseGroup::operator()(const Solution& current_solution) {
    std::unique_ptr<Solution> best;
    //std::cout << "dg_test" << std::endl;
    //std::cout << current_solution << std::endl;
    auto& destroy = select_destroy();
    for (size_t i = 0; i < /*40*/10; ++i) {
        auto neighborhood = std::make_unique<Solution>(current_solution);
        auto& dummy_group = neighborhood->get_dummy_group();

        //?j???̎??s
        auto destroy_items = destroy(*neighborhood);
        vector<MoveItem> destroy_move;
        destroy_move.reserve(destroy_items.size());
        for (auto&& item : destroy_items) {
            destroy_move.push_back(MoveItem(*item, neighborhood->get_group_id(*item), dummy_group.get_id()));
        }
        neighborhood->move(destroy_move);

        vector<MoveItem> move_items;
        move_items.reserve(dummy_group.get_member_num());
        vector<vector<const Item*>> add_members(Group::N);
        for (auto&& id : dummy_group.get_member_list()) {
            int assign_group_id = 0;
            double min_penalty = DBL_MAX;
            for (auto&& group : neighborhood->get_valid_groups()) {
                vector<const Item*> tmp = add_members[group->get_id()];
                tmp.push_back(&items[id]);
                double diff_penalty = 0;
                if (neighborhood->get_eval_flags().test(Solution::EvalIdx::WEIGHT_PENA)) {
                    diff_penalty += group->diff_weight_penalty(tmp, {}) - group->diff_weight_penalty(add_members[group->get_id()], {});
                }
                if (diff_penalty < min_penalty) {
                    min_penalty = diff_penalty;
                    assign_group_id = group->get_id();
                }
            }
            move_items.push_back(MoveItem(items[id], neighborhood->get_group_id(items[id]), assign_group_id));
            add_members[assign_group_id].push_back(&items[id]);
        }
        neighborhood->move(move_items);

        if (!best || best->get_eval_value() < neighborhood->get_eval_value()) {
            best = std::move(neighborhood);
            is_move = (move_items.size() > 0);
        }
    }
    //std::cout << *best << std::endl;

    return std::move(best);
}