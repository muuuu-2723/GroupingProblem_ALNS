#ifndef DESTROY_H_
#define DESTROY_H_

#include <Weight.hpp>
#include <vector>

class Item;
class Solution;

/*�j��@�̊��N���X*/
class Destroy {
protected:
    const std::vector<Item>& items;                                     //�O���[�v��������A�C�e���̏W��
    Weight weight;                                                      //�I���m�������߂�d��
public:
    /*�R���X�g���N�^*/
    Destroy(const std::vector<Item>& items, double init_weight, int param) : items(items), weight(init_weight ,param) {}
    /*����j��*/
    virtual void operator()(Solution& solution) {}
    virtual void update_weight(double score) final;
    virtual double get_weight() const final;
};

inline void Destroy::update_weight(double score) {
    weight.update(score);
}

inline double Destroy::get_weight() const {
    return weight.get_weight();
}

#include "RandomDestroy\RandomDestroy.hpp"
#include "RandomGroupDestroy\RandomGroupDestroy.hpp"
#include "MinimumDestroy\MinimumDestroy.hpp"
#include "MinimumGroupDestroy\MinimumGroupDestroy.hpp"
#include "UpperWeightGreedyDestroy\UpperWeightGreedyDestroy.hpp"

#endif