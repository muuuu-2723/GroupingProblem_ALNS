#include "NeighborhoodGraph.hpp"
#include <Search.hpp>
#include <Solution.hpp>
#include <Person.hpp>
#include <Group.hpp>
#include <Destroy.hpp>
#include <vector>
#include <cmath>
#include <cfloat>
#include <tuple>
#include <algorithm>
#include <memory>
#include <cassert>
#include <typeinfo>

using std::vector;

NeighborhoodGraph::NeighborhoodGraph(vector<Person>& persons, int param) : Search(persons, param) {
    vertices.reserve(Person::N + Group::N);
    int idx = 0;
    for (auto&& person : persons) {
        if (!person.is_leader) {
            vertices.push_back(Vertex(idx++, person));
        }
    }
    dummy_persons.reserve(Group::N);
    for (int i = 0; i < Group::N; ++i) {
        Person p;
        p.id = Person::N + i;
        dummy_persons.push_back(p);
        vertices.push_back(Vertex(idx++, dummy_persons[i]));
    }
}

void NeighborhoodGraph::set_edge(Solution& solution) {
    graph.assign(vertices.size(), vector<Edge>());
    for (const auto& s : vertices) {
        for (const auto& t : vertices) {
            if (s.person.id >= Person::N && t.person.id >= Person::N) continue;
            int s_group_id = s.person.id < Person::N ? solution.get_group_id(s.person) : s.person.id - Person::N;
            int t_group_id = t.person.id < Person::N ? solution.get_group_id(t.person) : t.person.id - Person::N;

            if (s_group_id != t_group_id) {
                const Group& s_group = solution.get_groups()[s_group_id];
                const Group& t_group = solution.get_groups()[t_group_id];
                if (s.person.id >= Person::N) {
                    if (t_group.diff_penalty({}, {&t.person}) == 0) {
                        double weight = 0;
                        weight -= solution.get_group_relation(t.person, t_group_id) * solution.get_relation_parameter();
                        //weight -= t_group.group_relation(t.person) * solution.get_relation_parameter();
                        weight -= solution.get_group_score_distance(t.person, t_group_id) * solution.get_balance_parameter();
                        //weight -= t_group.group_score_distance(t.person) * solution.get_balance_parameter();
                        double new_ave = (double)(t_group.get_sum_score() - t.person.score) / (t_group.get_member_num() - 1);
                        weight += std::abs(solution.get_ave() - t_group.score_average()) / Group::N * solution.get_score_parameter();
                        weight -= std::abs(solution.get_ave() - new_ave) / Group::N * solution.get_score_parameter();
                        graph[s.id].push_back(Edge(t.id, -weight));
                    }
                }
                else if (t.person.id >= Person::N) {
                    if (t_group.diff_penalty({&s.person}, {}) == 0) {
                        double weight = 0;
                        weight += solution.get_group_relation(s.person, t_group_id) * solution.get_relation_parameter();
                        //weight += t_group.group_relation(s.person) * solution.get_relation_parameter();
                        weight += solution.get_group_score_distance(s.person, t_group_id) * solution.get_balance_parameter();
                        //weight += t_group.group_score_distance(s.person) * solution.get_balance_parameter();
                        double new_ave = (double)(t_group.get_sum_score() + s.person.score) / (t_group.get_member_num() + 1);
                        weight += std::abs(solution.get_ave() - t_group.score_average()) / Group::N * solution.get_score_parameter();
                        weight -= std::abs(solution.get_ave() - new_ave) / Group::N * solution.get_score_parameter();
                        graph[s.id].push_back(Edge(t.id, -weight));
                    }
                }
                else if (t_group.diff_penalty({&s.person}, {&t.person}) == 0) {
                    double weight = 0;
                    weight += solution.get_group_relation(s.person, t_group_id) * solution.get_relation_parameter();
                    //weight += t_group.group_relation(s.person) * solution.get_relation_parameter();
                    weight += solution.get_group_score_distance(s.person, t_group_id) * solution.get_balance_parameter();
                    //weight += t_group.group_score_distance(s.person) * solution.get_balance_parameter();
                    weight -= solution.get_group_relation(t.person, t_group_id) * solution.get_relation_parameter();
                    //weight -= t_group.group_relation(t.person) * solution.get_relation_parameter();
                    weight -= solution.get_group_score_distance(t.person, t_group_id) * solution.get_balance_parameter();
                    //weight -= t_group.group_score_distance(t.person) * solution.get_balance_parameter();
                    weight -= (s.person.relations[t.person.id] - s.person.times[t.person.id]) * solution.get_relation_parameter();
                    weight -= s.person.score_distances[t.person.id] * solution.get_balance_parameter();
                    double new_ave = (double)(t_group.get_sum_score() - t.person.score + s.person.score) / t_group.get_member_num();
                    weight += std::abs(solution.get_ave() - t_group.score_average()) / Group::N * solution.get_score_parameter();
                    weight -= std::abs(solution.get_ave() - new_ave) / Group::N * solution.get_score_parameter();
                    graph[s.id].push_back(Edge(t.id, -weight));
                }
            }
        }
    }
}

Solution NeighborhoodGraph::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    assert(typeid(*destroy_ptr) == typeid(Destroy));
    Solution neighborhood_solution(current_solution);
    set_edge(neighborhood_solution);
    vector<vector<vector<double>>> dp(vertices.size(), vector<vector<double>>(vertices.size(), vector<double>(Group::N - 1, DBL_MAX)));
    using TablePos = std::tuple<int, int, int>;
    vector<vector<vector<TablePos>>> prev(vertices.size(), vector<vector<TablePos>>(vertices.size(), vector<TablePos>(Group::N - 1, {-1, -1, -1})));
    auto lambda = [](const double& a) { return a < 0 ? a : DBL_MAX; };
    
    for (const auto& v1 : vertices) {
        for (const auto& e : graph[v1.id]) {
            dp[v1.id][e.target][0] = lambda(e.weight);
        }

        for (int l = 1; l < Group::N - 1; ++l) {
            for (const auto& v2 : vertices) {
                for (const auto& e : graph[v2.id]) {
                    if (dp[v1.id][v2.id][l - 1] != DBL_MAX && lambda(dp[v1.id][v2.id][l - 1] + e.weight) < dp[v1.id][e.target][l]) {
                        dp[v1.id][e.target][l] = lambda(dp[v1.id][v2.id][l - 1] + e.weight);
                        prev[v1.id][e.target][l] = {v1.id, v2.id, l - 1};
                    }
                }
            }
        }
    }

    vector<std::pair<double, TablePos>> start_pos;
    for (const auto& v : vertices) {
        for (const auto& e : graph[v.id]) {
            for (int l = 0; l < Group::N - 1; ++l) {
                if (dp[e.target][v.id][l] + e.weight < 0) {
                    start_pos.push_back(std::make_pair(dp[e.target][v.id][l] + e.weight, std::make_tuple(e.target, v.id, l)));
                }
            }
        }
    }
    
    std::sort(start_pos.begin(), start_pos.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    for (const auto& sp : start_pos) {
        //std::cerr << sp.first << std::endl;
        vector<int> cycle;
        auto [v1, v2, l] = sp.second;
        cycle.reserve(l + 2);
        cycle.push_back(v1);
        auto pos = sp.second;
        while (std::get<0>(pos) != -1) {
            auto [now_v1, now_v2, now_l] = pos;
            cycle.push_back(now_v2);
            pos = prev[now_v1][now_v2][now_l];
        }
        cycle.push_back(v1);

        vector<MovePerson> move_persons;
        move_persons.reserve(l + 1);
        bool is_duplicated = false;
        unsigned int flag = 0;
        for (auto ritr = cycle.rbegin(), rend = --cycle.rend(); ritr != rend; ++ritr) {
            Person& p = vertices[*ritr].person;
            int now_group_id;
            if (p.id < Person::N) {
                now_group_id = neighborhood_solution.get_group_id(p);
                Person& next_p = vertices[*std::next(ritr)].person;
                int next_group_id;
                if (next_p.id < Person::N) {
                    next_group_id = neighborhood_solution.get_group_id(next_p);
                }
                else {
                    next_group_id = next_p.id - Person::N;
                }
                move_persons.push_back(MovePerson(p, now_group_id, next_group_id));
            }
            else {
                now_group_id = p.id - Person::N;
            }

            if (!(flag & (1<<now_group_id))) {
                flag |= (1<<now_group_id);
            }
            else {
                is_duplicated = true;
                break;
            }
        }
        if (is_duplicated) continue;

        //std::cerr << sp.first << std::endl;
        if (neighborhood_solution.move_check(move_persons)) break;
    }
    return neighborhood_solution;
}