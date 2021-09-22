#include "MinimumDestroy.hpp"
#include <Destroy.hpp>
#include <Solution.hpp>
#include <Group.hpp>
#include <vector>
#include <tuple>
#include <algorithm>

using std::vector;

void MinimumDestroy::operator()(Solution& solution) {
    vector<std::pair<int, int>> person_eval;
    person_eval.reserve(Person::N);
    for (const auto& p : persons) {
        if (!p.is_leader) {
            const Group& g = solution.get_groups()[solution.get_group_id(p)];
            int eval = solution.get_group_relation(p, g.get_id()) * solution.get_relation_parameter();
            eval += solution.get_group_score_distance(p, g.get_id()) * solution.get_balance_parameter();
            eval += g.diff_penalty({}, {&p}) * solution.get_penalty_parameter();
            person_eval.push_back({eval, p.id});
        }
    }

    std::sort(person_eval.begin(), person_eval.end());

    vector<MovePerson> move_persons;
    move_persons.reserve(destroy_num);
    for (int i = 0; i < destroy_num; ++i) {
        Person& p = persons[person_eval[i].second];
        move_persons.push_back(MovePerson(p, solution.get_group_id(p), Group::N));
    }

    solution.move(move_persons);
}