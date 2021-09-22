#include "RelationGreedy.hpp"
#include <Solution.hpp>
#include <Group.hpp>
#include <Destroy.hpp>
#include <Person.hpp>
#include <MyRandom.hpp>
#include <vector>
#include <cfloat>
#include <memory>

using std::vector;

Solution RelationGreedy::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    std::unique_ptr<Solution> best;
    for (int i = 0; i < /*40*/5; ++i) {
        std::unique_ptr<Solution> neighborhood_solution(new Solution(current_solution));
        (*destroy_ptr)(*neighborhood_solution);

        const auto& member_list = neighborhood_solution->get_dummy_group().get_member_list();
        vector<int> shuffle_member(member_list.begin(), member_list.end());
        MyRandom::shuffle(shuffle_member);

        for (const auto& id : shuffle_member) {
            int max_group_id;
            int member_num = INT_MAX;
            double max_value = -DBL_MAX;
            auto [group_begin, group_end] = neighborhood_solution->get_groups_range();
            for (auto citr = group_begin; citr != group_end; ++citr) {
                int group_member_num = citr->get_member_num();
                double value = neighborhood_solution->get_group_relation(persons[id], citr->get_id()) / (double)group_member_num;
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
            neighborhood_solution->move({MovePerson(persons[id], neighborhood_solution->get_group_id(persons[id]), max_group_id)});
        }

        if (!best || best->get_eval_value() < neighborhood_solution->get_eval_value()) {
            best = std::move(neighborhood_solution);
        }
    }

    return std::move(*best);
}