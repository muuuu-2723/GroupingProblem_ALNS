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

/*コンストラクタ*/
NeighborhoodGraph::NeighborhoodGraph(const vector<Item>& items, double init_weight, int param) : Search(items,init_weight , param) {
    vertices.reserve(Item::N + Group::N);
    int idx = 0;
    for (auto&& item : items) {
        if (item.predefined_group == -1) {
            vertices.push_back(Vertex(idx++, item));
        }
    }
    dummy_items.reserve(Group::N);
    //ダミーアイテムの設定. アイテムidをアイテム数以上に設定
    for (size_t i = 0; i < Group::N; ++i) {
        Item item;
        item.id = Item::N + i;
        dummy_items.push_back(item);
        vertices.push_back(Vertex(idx++, dummy_items[i]));
    }
}

/*各頂点間に有効辺を必要に応じて設定*/
void NeighborhoodGraph::set_edge(Solution& solution) {
    graph.assign(vertices.size(), vector<Edge>());

    //sからtへの有効辺を設定
    for (const auto& s : vertices) {
        for (const auto& t : vertices) {
            //ダミーアイテム同士には辺を張らない
            if (s.item.id >= Item::N && t.item.id >= Item::N) continue;

            int s_group_id = s.item.id < Item::N ? solution.get_group_id(s.item) : s.item.id - Item::N;
            int t_group_id = t.item.id < Item::N ? solution.get_group_id(t.item) : t.item.id - Item::N;

            //同じグループのアイテム間には辺を張らない
            if (s_group_id != t_group_id) {
                const Group& t_group = solution.get_groups()[t_group_id];
                if (s.item.id >= Item::N) {                                                 //sがダミーアイテムの場合
                    //tを現在のグループから削除したときのペナルティを計算
                    double penalty = 0;
                    if (solution.get_eval_flags().test(Solution::EvalIdx::WEIGHT_PENA)) {
                        penalty += t_group.diff_weight_penalty({}, {&t.item});
                    }
                    if (solution.get_eval_flags().test(Solution::EvalIdx::GROUP_PENA)) {
                        penalty -= t.item.group_penalty[t_group_id];
                    }
                    if (solution.get_eval_flags().test(Solution::EvalIdx::ITEM_PENA)) {
                        penalty -= solution.get_each_group_item_penalty(t.item, t_group_id);
                    }

                    //ペナルティが増加しない場合に辺を張る
                    if (penalty <= 0 || std::abs(penalty) < 1e-10) {
                        //評価値の変化量を計算
                        double weight = solution.calc_diff_eval(solution.evaluation_diff({MoveItem(t.item, t_group_id, Group::N)}));

                        //辺をグラフに追加
                        graph[s.id].push_back(Edge(t.id, -weight));
                    }
                }
                else if (t.item.id >= Item::N) {                                            //tがダミーアイテムの場合
                    //sをt_groupに追加したときのペナルティを計算
                    double penalty = 0;
                    if (solution.get_eval_flags().test(Solution::EvalIdx::WEIGHT_PENA)) {
                        penalty += t_group.diff_weight_penalty({&s.item}, {});
                    }
                    if (solution.get_eval_flags().test(Solution::EvalIdx::GROUP_PENA)) {
                        penalty += s.item.group_penalty[t_group_id];
                    }
                    if (solution.get_eval_flags().test(Solution::EvalIdx::ITEM_PENA)) {
                        penalty += solution.get_each_group_item_penalty(s.item, t_group_id);
                    }

                    //ペナルティが増加しない場合に辺を張る
                    if (penalty <= 0 || std::abs(penalty) < 1e-10) {
                        //評価値の変化量を計算
                        double weight = solution.calc_diff_eval(solution.evaluation_diff({MoveItem(s.item, Group::N, t_group_id)}));

                        //辺をグラフに追加
                        graph[s.id].push_back(Edge(t.id, -weight));
                    }
                }
                else {                                                                      //sとtがダミーアイテムでない場合
                    
                    //tを現在のグループから削除し, sをそのグループに追加したときのペナルティを計算
                    double penalty = 0;
                    if (solution.get_eval_flags().test(Solution::EvalIdx::WEIGHT_PENA)) {
                        penalty += t_group.diff_weight_penalty({&s.item}, {&t.item});
                    }
                    if (solution.get_eval_flags().test(Solution::EvalIdx::GROUP_PENA)) {
                        penalty += s.item.group_penalty[t_group_id] - t.item.group_penalty[t_group_id];
                    }
                    if (solution.get_eval_flags().test(Solution::EvalIdx::ITEM_PENA)) {
                        penalty += solution.get_each_group_item_penalty(s.item, t_group_id) - solution.get_each_group_item_penalty(t.item, t_group_id) - s.item.item_penalty[t.item.id];
                    }
                    
                    //ペナルティが増加しない場合に辺を張る
                    if (penalty <= 0 || std::abs(penalty) < 1e-10) {
                        //評価値の変化量を計算
                        double weight = solution.calc_diff_eval(solution.evaluation_diff({MoveItem(s.item, Group::N, t_group_id), MoveItem(t.item, t_group_id, Group::N)}));

                        //辺をグラフに追加
                        graph[s.id].push_back(Edge(t.id, -weight));
                    }
                }
            }
        }
    }
}

/*
 *グラフを探索
 *グラフ上の同じグループを2回通らない負閉路を探索
 *上記の負閉路の移動を評価関数で評価し, 改善していれば移動する
 *destroy_ptrがDestroy以外の場合, エラー
 */
std::unique_ptr<Solution> NeighborhoodGraph::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    assert(typeid(*destroy_ptr) == typeid(Destroy));

    auto neighborhood_solution = std::make_unique<Solution>(current_solution);
    size_t valid_group_size = neighborhood_solution->get_valid_groups().size();
    std::cerr << "nei_test" << std::endl;
    set_edge(*neighborhood_solution);
    vector<vector<vector<double>>> dp(vertices.size(), vector<vector<double>>(vertices.size(), vector<double>(valid_group_size - 1, DBL_MAX)));
    using TablePos = std::tuple<int, int, int>;
    vector<vector<vector<TablePos>>> prev(vertices.size(), vector<vector<TablePos>>(vertices.size(), vector<TablePos>(valid_group_size - 1, {-1, -1, -1})));
    
    //探索の効率化のためのラムダ関数
    auto lambda = [](const double& a) { return a < 0 ? a : DBL_MAX; };
    
    //DPで負閉路を探索. 負閉路の長さはグループ数以下のものが探索の対象
    if (valid_group_size > 1) {
        for (const auto& v1 : vertices) {
            for (const auto& e : graph[v1.id]) {
                dp[v1.id][e.target][0] = lambda(e.weight);
            }

            for (size_t l = 1, size = valid_group_size - 1; l < size; ++l) {
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
    }
    

    //DPテーブルから負閉路毎に重みの合計と始点を抽出
    vector<std::pair<double, TablePos>> start_pos;
    for (const auto& v : vertices) {
        for (const auto& e : graph[v.id]) {
            for (size_t l = 0, size = valid_group_size - 1; l < size; ++l) {
                if (dp[e.target][v.id][l] + e.weight < 0) {
                    start_pos.push_back(std::make_pair(dp[e.target][v.id][l] + e.weight, std::make_tuple(e.target, v.id, l)));
                }
            }
        }
    }
    
    //重みで負閉路をソート
    std::sort(start_pos.begin(), start_pos.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    //負閉路を構築し, 同じグループを2回以上通っていないか調査
    for (const auto& sp : start_pos) {
        //負閉路構築
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
        bool is_duplicated = false;                     //同じグループを2回以上通っているかを表すbool型
        unsigned int flag = 0;                          //グループを通ったかを管理するビットフラグ

        //同じグループを2回以上通っていないか調査
        for (auto ritr = cycle.rbegin(), rend = --cycle.rend(); ritr != rend; ++ritr) {
            const Item& item = vertices[*ritr].item;
            int now_group_id;
            if (item.id < Item::N) {
                now_group_id = neighborhood_solution->get_group_id(item);
                const Item& next_item = vertices[*std::next(ritr)].item;
                int next_group_id;
                if (next_item.id < Item::N) {
                    next_group_id = neighborhood_solution->get_group_id(next_item);
                }
                else {
                    next_group_id = next_item.id - Item::N;
                }
                move_items.push_back(MoveItem(item, now_group_id, next_group_id));
            }
            else {
                now_group_id = item.id - Item::N;
            }

            if (!(flag & (1<<now_group_id))) {              //itemのグループにフラグが立っていない場合
                //フラグを立てる
                flag |= (1<<now_group_id);
            }
            else {                                          //itemのグループにすでにフラグが立っている場合
                is_duplicated = true;
                break;
            }
        }
        /*if (is_duplicated) continue;

        //負閉路の移動が改善解になっている場合, 移動してループを抜ける
        if (neighborhood_solution->move_check(move_items)) break;*/
        if (!is_duplicated) {
            std::cout << move_items.size() << std::endl;
            neighborhood_solution->move(move_items);
            break;
        }
    }
    return std::move(neighborhood_solution);
}