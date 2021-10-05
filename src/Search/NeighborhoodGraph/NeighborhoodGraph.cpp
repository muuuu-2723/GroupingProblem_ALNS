#include "NeighborhoodGraph.hpp"
#include <Search.hpp>
#include <Solution.hpp>
#include <Item.hpp>
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

NeighborhoodGraph::NeighborhoodGraph(const vector<Item>& items, double init_weight, int param) : Search(items,init_weight , param) {
    vertices.reserve(Item::N + Group::N);
    int idx = 0;
    for (auto&& item : items) {
        if (item.predefined_group == -1) {
            vertices.push_back(Vertex(idx++, item));
        }
    }
    dummy_items.reserve(Group::N);
    for (size_t i = 0; i < Group::N; ++i) {
        Item item;
        item.id = Item::N + i;
        dummy_items.push_back(item);
        vertices.push_back(Vertex(idx++, dummy_items[i]));
    }
}

void NeighborhoodGraph::set_edge(Solution& solution) {
    graph.assign(vertices.size(), vector<Edge>());
    for (const auto& s : vertices) {
        for (const auto& t : vertices) {
            if (s.item.id >= Item::N && t.item.id >= Item::N) continue;
            int s_group_id = s.item.id < Item::N ? solution.get_group_id(s.item) : s.item.id - Item::N;
            int t_group_id = t.item.id < Item::N ? solution.get_group_id(t.item) : t.item.id - Item::N;

            if (s_group_id != t_group_id) {
                const Group& t_group = solution.get_groups()[t_group_id];
                if (s.item.id >= Item::N) {
                    double penalty = t_group.diff_weight_penalty({}, {&t.item});
                    penalty -= t.item.group_penalty[t_group_id];
                    penalty -= solution.get_each_group_item_penalty(t.item, t_group_id);
                    if (penalty <= 0 || std::abs(penalty) < 1e-10) {
                        double weight = 0;
                        for (auto&& r : solution.get_each_group_item_relation(t.item, t_group_id)) {
                            weight -= r * solution.get_relation_parameter();
                        }
                        for (auto&& r : t.item.group_relations[t_group_id]) {
                            weight -= r * solution.get_relation_parameter();
                        }
                        for (size_t i = 0; i < Item::v_size; ++i) {
                            weight -= std::abs(solution.get_ave()[i] - t_group.value_average(i)) / Group::N * solution.get_ave_balance_parameter();
                            double new_ave = (t_group.get_sum_values()[i] - t.item.values[i]) / (t_group.get_member_num() - 1);
                            weight += std::abs(solution.get_ave()[i] - new_ave) / Group::N * solution.get_ave_balance_parameter();

                            weight -= std::abs((solution.get_sum_values()[i] / Group::N) - t_group.get_sum_values()[i]) * solution.get_sum_balance_parameter();
                            weight += std::abs((solution.get_sum_values()[i] / Group::N) - (t_group.get_sum_values()[i] - t.item.values[i])) * solution.get_sum_balance_parameter();
                        }
                        graph[s.id].push_back(Edge(t.id, -weight + penalty * solution.get_penalty_parameter()));
                    }
                }
                else if (t.item.id >= Item::N) {
                    double penalty = t_group.diff_weight_penalty({&s.item}, {});
                    penalty += s.item.group_penalty[t_group_id];
                    penalty += solution.get_each_group_item_penalty(s.item, t_group_id);
                    if (penalty <= 0 || std::abs(penalty) < 1e-10) {
                        double weight = 0;
                        for (auto&& r : solution.get_each_group_item_relation(s.item, t_group_id)) {
                            weight += r * solution.get_relation_parameter();
                        }
                        for (auto&& r : s.item.group_relations[t_group_id]) {
                            weight += r * solution.get_relation_parameter();
                        }
                        for (size_t i = 0; i < Item::v_size; ++i) {
                            weight -= std::abs(solution.get_ave()[i] - t_group.value_average(i)) / Group::N * solution.get_ave_balance_parameter();
                            double new_ave = (t_group.get_sum_values()[i] + s.item.values[i]) / (t_group.get_member_num() + 1);
                            weight += std::abs(solution.get_ave()[i] - new_ave) / Group::N * solution.get_ave_balance_parameter();

                            weight -= std::abs((solution.get_sum_values()[i] / Group::N) - t_group.get_sum_values()[i]) * solution.get_sum_balance_parameter();
                            weight += std::abs((solution.get_sum_values()[i] / Group::N) - (t_group.get_sum_values()[i] + s.item.values[i])) * solution.get_sum_balance_parameter();
                        }
                        graph[s.id].push_back(Edge(t.id, -weight + penalty * solution.get_penalty_parameter()));
                    }
                }
                else {
                    double penalty = t_group.diff_weight_penalty({&s.item}, {&t.item});
                    penalty += s.item.group_penalty[t_group_id] - t.item.group_penalty[t_group_id];
                    penalty += solution.get_each_group_item_penalty(s.item, t_group_id) - solution.get_each_group_item_penalty(t.item, t_group_id) - s.item.item_penalty[t.item.id];
                    if (penalty <= 0 || std::abs(penalty) < 1e-10) {
                        double weight = 0;
                        for (auto&& r : solution.get_each_group_item_relation(t.item, t_group_id)) {
                            weight -= r * solution.get_relation_parameter();
                        }
                        for (auto&& r : solution.get_each_group_item_relation(s.item, t_group_id)) {
                            weight += r * solution.get_relation_parameter();
                        }
                        for (size_t i = 0; i < Item::item_r_size; ++i) {
                            weight -= s.item.item_relations[t.item.id][i];
                        }

                        for (auto&& r : t.item.group_relations[t_group_id]) {
                            weight -= r * solution.get_relation_parameter();
                        }
                        for (auto&& r : s.item.group_relations[t_group_id]) {
                            weight += r * solution.get_relation_parameter();
                        }

                        for (size_t i = 0; i < Item::v_size; ++i) {
                            weight -= std::abs(solution.get_ave()[i] - t_group.value_average(i)) / Group::N * solution.get_ave_balance_parameter();
                            double new_ave = (t_group.get_sum_values()[i] + s.item.values[i] - t.item.values[i]) / t_group.get_member_num();
                            weight += std::abs(solution.get_ave()[i] - new_ave) / Group::N * solution.get_ave_balance_parameter();

                            weight -= std::abs((solution.get_sum_values()[i] / Group::N) - t_group.get_sum_values()[i]) * solution.get_sum_balance_parameter();
                            weight += std::abs((solution.get_sum_values()[i] / Group::N) - (t_group.get_sum_values()[i] + s.item.values[i] - t.item.values[i])) * solution.get_sum_balance_parameter();
                        }
                        graph[s.id].push_back(Edge(t.id, -weight + penalty * solution.get_penalty_parameter()));
                    }
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

        vector<MoveItem> move_items;
        move_items.reserve(l + 1);
        bool is_duplicated = false;
        unsigned int flag = 0;
        for (auto ritr = cycle.rbegin(), rend = --cycle.rend(); ritr != rend; ++ritr) {
            const Item& item = vertices[*ritr].item;
            int now_group_id;
            if (item.id < Item::N) {
                now_group_id = neighborhood_solution.get_group_id(item);
                const Item& next_item = vertices[*std::next(ritr)].item;
                int next_group_id;
                if (next_item.id < Item::N) {
                    next_group_id = neighborhood_solution.get_group_id(next_item);
                }
                else {
                    next_group_id = next_item.id - Item::N;
                }
                move_items.push_back(MoveItem(item, now_group_id, next_group_id));
            }
            else {
                now_group_id = item.id - Item::N;
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
        if (neighborhood_solution.move_check(move_items)) break;
    }
    return neighborhood_solution;
}