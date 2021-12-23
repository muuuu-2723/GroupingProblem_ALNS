#ifndef NEIGHBORHOODGRAPH_H_
#define NEIGHBORHOODGRAPH_H_

#include <vector>
#include <memory>
#include <Search.hpp>

struct Item;
class Solution;
class Destroy;

/*���P��T���O���t��T��*/
class NeighborhoodGraph : public Search {
    /*NeighborhoodGraph�p�̒��_*/
    struct Vertex {
        int id;                             //���_�ԍ�
        const Item& item;                   //���_�ɕR�Â��A�C�e��
        Vertex(int id, const Item& item) : id(id), item(item) {}
    };

    /*NeighborhoodGraph�p�̗L����*/
    struct Edge {
        int source;                         //�L���ӂ̎n�_�̒��_�ԍ�
        double weight;                      //�L���ӂ̏d��
        Edge(int source, double weight) : source(source), weight(weight) {}
    };

private:
    std::vector<Vertex> vertices;                       //���_�W��
    std::vector<std::vector<Edge>> graph;               //�O���t�̋t�אڃ��X�g
    std::vector<Item> dummy_items;                      //��̈ړ��݂̂łȂ��p�X��̈ړ����l���邽�߂̃_�~�[�A�C�e���̏W��
    double destroy_ratio;
    void set_edge(Solution& solution);                  //�e���_�ԂɗL���ӂ�K�v�ɉ����Đݒ�
public:
    NeighborhoodGraph(const std::vector<Item>& items, double init_weight, int param, const Solution& solution);     //�R���X�g���N�^
    std::unique_ptr<Solution> operator()(const Solution& current_solution) override;                                //�O���t��T��
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