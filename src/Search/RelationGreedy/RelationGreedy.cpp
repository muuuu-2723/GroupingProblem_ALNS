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

Solution RelationGreedy::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    std::unique_ptr<Solution> best;
    for (size_t i = 0; i < /*40*/5; ++i) {
        std::unique_ptr<Solution> neighborhood(new Solution(current_solution));
        (*destroy_ptr)(*neighborhood);

        const auto& member_list = neighborhood->get_dummy_group().get_member_list();
        vector<int> shuffle_member(member_list.begin(), member_list.end());
        MyRandom::shuffle(shuffle_member);

        for (const auto& id : shuffle_member) {
            int max_group_id;
            int member_num = INT_MAX;
            double max_value = -DBL_MAX;
            auto [group_begin, group_end] = neighborhood->get_groups_range();
            for (auto citr = group_begin; citr != group_end; ++citr) {
                int group_member_num = citr->get_member_num();
                auto& item_relations = neighborhood->get_each_group_item_relation(items[id], citr->get_id());
                double value = std::accumulate(item_relations.begin(), item_relations.end(), 0.0);
                value += std::accumulate(items[id].group_relations[citr->get_id()].begin(), items[id].group_relations[citr->get_id()].end(), 0.0);
                value /= group_member_num;
                if (member_num > group_member_num) {
                    member_num = group_member_num;
                    max_value = value;
                    max_group_id = citr->get_id();
                }
                else if (member_num == group_member_num) {
                    if (value > max_value) {
                        max_value = value;
                        max_group_id = citr->get_id();
                    }
                }
            }
            neighborhood->move({MoveItem(items[id], neighborhood->get_group_id(items[id]), max_group_id)});
        }

        if (!best || best->get_eval_value() < neighborhood->get_eval_value()) {
            best = std::move(neighborhood);
        }
    }

    return std::move(*best);
}