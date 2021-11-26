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
        int target;                         //有効辺の終点の頂点番号
        double weight;                      //有効辺の重み
        Edge(int target, double weight) : target(target), weight(weight) {}
    };

private:
    std::vector<std::shared_ptr<Vertex>> vertices;      //頂点集合
    std::vector<std::shared_ptr<Vertex>> target_vertices;
    std::vector<std::vector<Edge>> graph;               //グラフの隣接リスト
    std::vector<Item> dummy_items;                      //環状の移動のみでなくパス状の移動を考えるためのダミーアイテムの集合
    void set_edge(Solution& solution);                  //各頂点間に有効辺を必要に応じて設定
public:
    NeighborhoodGraph(const std::vector<Item>& items, double init_weight, int param);                           //コンストラクタ
    std::unique_ptr<Solution> operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;       //グラフを探索
};

#endif