#include "SwapNeighborhood.hpp"
#include <Search.hpp>
#include <Solution.hpp>
#include <MyRandom.hpp>
#include <Person.hpp>
#include <Group.hpp>
#include <Destroy.hpp>
#include <vector>
#include <algorithm>
#include <cassert>
#include <typeinfo>
#include <memory>

using std::vector;

SwapNeighborhood::SwapNeighborhood(vector<Person>& persons, int param) : Search(persons, param) {
    shuffle_member_ids1.reserve(Person::N - Group::N);
    for (const auto& person : persons) {
        if (!person.is_leader) {
            shuffle_member_ids1.push_back(person.id);
        }
    }
    shuffle_member_ids2.resize(shuffle_member_ids1.size());
    std::copy(shuffle_member_ids1.begin(), shuffle_member_ids1.end(), shuffle_member_ids2.begin());
}

Solution SwapNeighborhood::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    assert(typeid(*destroy_ptr) == typeid(Destroy));
    /*MyRandom::shuffle(shuffle_member_ids1);
    MyRandom::shuffle(shuffle_member_ids2);

    auto p1_itr = shuffle_member_ids1.begin();
    auto p2_itr = shuffle_member_ids2.begin();
    auto p1_start = p1_itr;
    auto p2_start = p2_itr;
    

    Solution neighborhood_solution(current_solution);
    do {
        Person& p1 = persons[*p1_itr];
        do {
            Person& p2 = persons[*p2_itr];
            if (p1.id != p2.id && neighborhood_solution.get_group_id(p1) != neighborhood_solution.get_group_id(p2)) {
                if (neighborhood_solution.swap_check(p1, p2)) {
                    //p1_start = p1_itr;
                    //p2_start = p2_itr;
                    return neighborhood_solution;
                }
            }

            ++p2_itr;
            if (p2_itr == shuffle_member_ids2.end()) {
                p2_itr = shuffle_member_ids2.begin();
            }
        } while (p2_itr != p2_start);
        
        ++p1_itr;
        if (p1_itr == shuffle_member_ids1.end()) {
            p1_itr = shuffle_member_ids1.begin();
        }
    } while (p1_itr != p1_start);
    
    return neighborhood_solution;*/

    /*std::unique_ptr<Solution> best;
    std::unique_ptr<Solution> neighborhood_solution(new Solution(current_solution));
    for (auto p1_itr = persons.begin(), end = persons.end(); p1_itr != end; ++p1_itr) {
        if (p1_itr->is_leader) continue;
        for (auto p2_itr = std::next(p1_itr); p2_itr != end; ++p2_itr) {
            if (p2_itr->is_leader) continue;
            if (neighborhood_solution->get_group_id(*p1_itr) != neighborhood_solution->get_group_id(*p2_itr)) {
                if (neighborhood_solution->swap_check(*p1_itr, *p2_itr)) {
                    if (!best || best->get_eval_value() < neighborhood_solution->get_eval_value()) {
                        best = std::move(neighborhood_solution);
                    }
                    neighborhood_solution.reset(new Solution(current_solution));
                }
            }
        }
    }

    if (!best) return current_solution;
    return std::move(*best);*/

    Solution neighborhood_solution(current_solution);
    double max_diff = 0;
    int max_p1_id, max_p2_id;
    for (auto p1_itr = shuffle_member_ids1.begin(), end = shuffle_member_ids1.end(); p1_itr != end; ++p1_itr) {
        Person& p1 = persons[*p1_itr];
        for (auto p2_itr = std::next(p1_itr); p2_itr != end; ++p2_itr) {
            Person& p2 = persons[*p2_itr];
            if (neighborhood_solution.get_group_id(p1) != neighborhood_solution.get_group_id(p2)) {
                auto [diff_r, diff_p, diff_d, diff_b] = neighborhood_solution.evaluation_swap(p1, p2);
                double diff_eval = diff_r * neighborhood_solution.get_relation_parameter() - diff_p * neighborhood_solution.get_penalty_parameter() - diff_d * neighborhood_solution.get_score_parameter() + diff_b * neighborhood_solution.get_balance_parameter();
                if (diff_eval > max_diff) {
                    max_p1_id = p1.id;
                    max_p2_id = p2.id;
                    max_diff = diff_eval;
                }
            }
        }
    }

    if (max_diff != 0) {
        neighborhood_solution.swap_check(persons[max_p1_id], persons[max_p2_id]);
    }
    return neighborhood_solution;
}