#include "PenaltyGreedy.hpp"
#include <Solution.hpp>
#include <Group.hpp>
#include <Destroy.hpp>
#include <Person.hpp>
#include <memory>
#include <vector>
#include <climits>
#include <iostream>
#include <list>
#include <typeinfo>

using std::vector;

Solution PenaltyGreedy::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    std::unique_ptr<Solution> best;
    int N = 7/*13*/;
    if (typeid(*destroy_ptr) == typeid(MinimumDestroy) || typeid(*destroy_ptr) == typeid(MinimumGroupDestroy)) {
        N = 1;
    }
    for (int i = 0; i < N; ++i) {
        std::unique_ptr<Solution> neighborhood_solution(new Solution(current_solution));
        (*destroy_ptr)(*neighborhood_solution);

        const Group& dummy_group = neighborhood_solution->get_dummy_group();
        vector<MovePerson> move_persons;
        move_persons.reserve(dummy_group.get_member_num());
        vector<vector<const Person*>> add_members(Group::N); 
        auto member_list = dummy_group.get_member_list();
        /*member_list.sort([&](const auto& a, const auto& b) { 
            Person& p1 = persons[a], p2 = persons[b];
            return p1.year == p2.year ? (p1.gender == p2.gender ? p1.campus < p2.campus : p1.gender < p2.gender) : p1.year < p2.year;
        });*/

        /*for (int year = 0; year < 3; ++year) {
            for (int gender = 0; gender < 2; ++gender) {
                for (int campus = 0; campus < 2; ++campus) {
                    //std::cerr << year << " " << gender << " " << campus << std::endl;
                    if (dummy_group.get_num_ratio(year, gender, campus) >= Group::N) {
                        auto itr = member_list.begin();
                        std::cerr << dummy_group.get_num_ratio(year, gender, campus) << std::endl;
                        for (int j = 0; j < dummy_group.get_num_ratio(year, gender, campus) / Group::N * Group::N; ++j) {
                            while (persons[*itr].year != year + 1 || persons[*itr].gender != gender || persons[*itr].campus != campus) {
                                ++itr;
                            }
                            std::cerr << std::distance(itr, member_list.end()) << std::endl;
                            move_persons.push_back(MovePerson(persons[*itr], neighborhood_solution->get_group_id(persons[*itr]), j % Group::N));
                            itr = member_list.erase(itr);
                        }
                    }
                }
            }
        }*/
        auto lower = Group::get_lower();
        for (auto itr = member_list.begin(); itr != member_list.end(); ++itr) {
            Person& p = persons[*itr];
            auto [group_begin, group_end] = neighborhood_solution->get_groups_range();
            for (auto citr = group_begin; citr != group_end; ++citr) {
                if (citr->get_num_ratio(p.year - 1, p.gender, p.campus) < lower[p.year - 1][p.gender][p.campus]) {
                    neighborhood_solution->move({MovePerson(p, neighborhood_solution->get_group_id(p), citr->get_id())});
                    itr = member_list.erase(itr);
                    break;
                }
            }
        }

        for (const auto& id : member_list) {
            int min_group_id = -1;
            int min_penalty = INT_MAX;
            auto [group_begin, group_end] = neighborhood_solution->get_groups_range();
            for (auto citr = group_begin; citr != group_end; ++citr) {
                vector<const Person*> tmp = add_members[citr->get_id()];
                tmp.push_back(&persons[id]);
                //for (const auto& p : tmp) std::cerr << p->id << " ";
                int diff_penalty = citr->diff_penalty(tmp, {}) - citr->diff_penalty(add_members[citr->get_id()], {});
                if (diff_penalty < min_penalty) {
                    min_penalty = diff_penalty;
                    min_group_id = citr->get_id();
                }
                /*else if (diff_penalty == min_penalty) {
                    if (group_num > citr->get_member_num() + tmp.size()) {
                        min_group_id = citr->get_id();
                        group_num = citr->get_member_num() + tmp.size();
                    }
                }*/
            }
            move_persons.push_back(MovePerson(persons[id], neighborhood_solution->get_group_id(persons[id]), min_group_id));
            add_members[min_group_id].push_back(&persons[id]);
        }

        /*auto member_list = dummy_group.get_member_list();
        member_list.sort([&](const auto& a, const auto& b) { 
            Person& p1 = persons[a], p2 = persons[b];
            return {p1.year, p1.gender, p1.campus} < {p2.year, p2.gender, p2.campus};
        });
        auto [group_begin, group_end] = neighborhood_solution->get_groups_range();
        auto g_itr = group_begin;
        for (auto itr = member_list.begin(); itr != member_list.end(); ++itr) {
            for ()
        }

        const auto& upper = Group::get_upper();
        const auto& lower = Group::get_lower();
        vector<vector<vector<vector<int>>>> category(upper.size(), vector<vector<vector<int>>>(upper[0].size(), vector<vector<int>>(upper[0][0].size(), vector<int>())));
        for (const auto& id : dummy_group.get_member_list()) {
            Person& p = persons[id];
            category[p.year - 1][p.gender][p.campus].push_back(id);
        }*/

        
        neighborhood_solution->move(move_persons);
        if (!best || best->get_eval_value() < neighborhood_solution->get_eval_value()) {
            best = std::move(neighborhood_solution);
        }
    }
    return std::move(*best);
}