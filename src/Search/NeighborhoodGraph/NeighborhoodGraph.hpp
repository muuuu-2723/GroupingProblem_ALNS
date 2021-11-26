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
        int target;                         //�L���ӂ̏I�_�̒��_�ԍ�
        double weight;                      //�L���ӂ̏d��
        Edge(int target, double weight) : target(target), weight(weight) {}
    };

private:
    std::vector<std::shared_ptr<Vertex>> vertices;      //���_�W��
    std::vector<std::shared_ptr<Vertex>> target_vertices;
    std::vector<std::vector<Edge>> graph;               //�O���t�̗אڃ��X�g
    std::vector<Item> dummy_items;                      //��̈ړ��݂̂łȂ��p�X��̈ړ����l���邽�߂̃_�~�[�A�C�e���̏W��
    void set_edge(Solution& solution);                  //�e���_�ԂɗL���ӂ�K�v�ɉ����Đݒ�
public:
    NeighborhoodGraph(const std::vector<Item>& items, double init_weight, int param);                           //�R���X�g���N�^
    std::unique_ptr<Solution> operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) override;       //�O���t��T��
};

#endif