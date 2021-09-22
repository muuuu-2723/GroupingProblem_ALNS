#include "MinimumGroupDestroy.hpp"
#include <Destroy.hpp>
#include <Solution.hpp>
#include <Group.hpp>
#include <vector>
#include <tuple>
#include <algorithm>

using std::vector;

void MinimumGroupDestroy::operator()(Solution& solution) {
    vector<std::pair<int, int>> group_eval;
    group_eval.reserve(Group::N);
    auto [group_begin, group_end] = solution.get_groups_range();
    for (auto citr = group_begin; citr != group_end; ++citr) {
        int eval = 0;
        vector<const Person*> tmp_member;
        tmp_member.reserve(citr->get_member_num());
        for (const auto& id : citr->get_member_list()) {
            if (persons[id].is_leader) continue;
            eval += solution.get_group_relation(persons[id], citr->get_id()) * solution.get_relation_parameter();
            eval += solution.get_group_score_distance(persons[id], citr->get_id()) * solution.get_balance_parameter();
            tmp_member.push_back(&persons[id]);
        }
        eval += citr->diff_penalty({}, tmp_member) * solution.get_penalty_parameter();

        group_eval.push_back({eval, citr->get_id()});
    }

    std::sort(group_eval.begin(), group_eval.end());

    vector<MovePerson> move_persons;
    for (int i = 0; i < destroy_num; ++i) {
        const Group& g = solution.get_groups()[group_eval[i].second];
        for (const auto& id : g.get_member_list()) {
            if (persons[id].is_leader) continue;
            move_persons.push_back(MovePerson(persons[id], g.get_id(), Group::N));
        }
    }

    solution.move(move_persons);
}