#ifndef NEIGHBORHOODGRAPH_H_
#define NEIGHBORHOODGRAPH_H_

#include <vector>
#include <memory>
#include <Search.hpp>

struct Item;
class Solution;
class Destroy;

/*改善解探索グラフを探索*/
class NeighborhoodGraph : public Search {
    /*NeighborhoodGraph用の頂点*/
    struct Vertex {
        int id;                             //頂点番号
        const Item& item;                   //頂点に紐づくアイテム
        Vertex(int id, const Item& item) : id(id), item(item) {}
    };

    /*NeighborhoodGraph用の有効辺*/
    struct Edge {
        int source;                         //有効辺の始点の頂点番号
        double weight;                      //有効辺の重み
        Edge(int source, double weight) : source(source), weight(weight) {}
    };

private:
    std::vector<Vertex> vertices;                       //頂点集合
    std::vector<std::vector<Edge>> graph;               //グラフの逆隣接リスト
    std::vector<Item> dummy_items;                      //環状の移動のみでなくパス状の移動を考えるためのダミーアイテムの集合
    double destroy_ratio;
    void set_edge(Solution& solution);                  //各頂点間に有効辺を必要に応じて設定
public:
    NeighborhoodGraph(const std::vector<Item>& items, double init_weight, int param, const Solution& solution);     //コンストラクタ
    std::unique_ptr<Solution> operator()(const Solution& current_solution) override;                                //グラフを探索
    void reset_destroy_num(const Solution& solution) override;
    void update_destroy_num(const Solution& solution) override;
};

inline void NeighborhoodGraph::reset_destroy_num(const Solution& solution) {
    int group_destroy_num = solution.get_valid_groups().size();
    for (auto&& d : group_destroy) {
        d->set_destroy_num(group_destroy_num, solution);
    }
}

inline void NeighborhoodGraph::update_destroy_num(const Solution& solution) {
    int group_destroy_num = solution.get_valid_groups().size();
    for (auto&& d : group_destroy) {
        d->set_destroy_num(group_destroy_num, solution);
    }
}

#endif