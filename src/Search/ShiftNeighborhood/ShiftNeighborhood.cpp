#include "ShiftNeighborhood.hpp"
#include <Search.hpp>
#include <Solution.hpp>
#include <MyRandom.hpp>
#include <Person.hpp>
#include <Group.hpp>
#include <Destroy.hpp>
#include <vector>
#include <numeric>
#include <cassert>
#include <typeinfo>
#include <memory>

using std::vector;

ShiftNeighborhood::ShiftNeighborhood(vector<Person>& persons, int param) : Search(persons, param) {
    shuffle_member_ids.reserve(Person::N - Group::N);
    for (const auto& person : persons) {
        if (!person.is_leader) {
            shuffle_member_ids.push_back(person.id);
        }
    }
    shuffle_group_ids.resize(Group::N);
    std::iota(shuffle_group_ids.begin(), shuffle_group_ids.end(), 0);
}

Solution ShiftNeighborhood::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    assert(typeid(*destroy_ptr) == typeid(Destroy));
    /*MyRandom::shuffle(shuffle_member_ids);
    MyRandom::shuffle(shuffle_group_ids);

    auto p_itr = shuffle_member_ids.begin();
    auto g_itr = shuffle_group_ids.begin();
    auto p_start = p_itr;
    auto g_start = g_itr;

    Solution neighborhood_solution(current_solution);
    do {
        do {
            Person& p = persons[*p_itr];
            if (*g_itr != neighborhood_solution.get_group_id(p) && neighborhood_solution.shift_check(p, *g_itr)) {
                //p_start = p_itr;
                //g_start = g_itr;
                return neighborhood_solution;
            }

            ++g_itr;
            if (g_itr == shuffle_group_ids.end()) {
                g_itr = shuffle_group_ids.begin();
            }
        } while (g_start != g_itr);
        
        ++p_itr;
        if (p_itr == shuffle_member_ids.end()) {
            p_itr = shuffle_member_ids.begin();
        }
    } while (p_start != p_itr);

    return neighborhood_solution;*/

    /*std::unique_ptr<Solution> best;
    std::unique_ptr<Solution> neighborhood_solution(new Solution(current_solution));
    for (auto&& p : persons) {
        if (p.is_leader) continue;
        for (int i = 0; i < Group::N; ++i) {
            if (i != neighborhood_solution->get_group_id(p) && neighborhood_solution->shift_check(p, i)) {
                if (!best || best->get_eval_value() < neighborhood_solution->get_eval_value()) {
                    best = std::move(neighborhood_solution);
                }
                neighborhood_solution.reset(new Solution(current_solution));
            }
        }
    }

    if (!best) return current_solution;
    return std::move(*best);*/

    Solution neighborhood_solution(current_solution);
    double max_diff = 0;
    int max_p_id, max_group_id;
    for (const auto& p_id : shuffle_member_ids) {
        Person& p = persons[p_id];
        for (const auto& g_id : shuffle_group_ids) {
            if (g_id != neighborhood_solution.get_group_id(p)) {
                auto [diff_r, diff_p, diff_d, diff_b] = neighborhood_solution.evaluation_shift(p, g_id);
                double diff_eval = diff_r * neighborhood_solution.get_relation_parameter() - diff_p * neighborhood_solution.get_penalty_parameter() - diff_d * neighborhood_solution.get_score_parameter() + diff_b * neighborhood_solution.get_balance_parameter();
                if (diff_eval > max_diff) {
                    max_p_id = p.id;
                    max_group_id = g_id;
                    max_diff = diff_eval;
                }
            }
        }
    }

    if (max_diff != 0) {
        neighborhood_solution.shift_check(persons[max_p_id], max_group_id);
    }

    return neighborhood_solution;
}