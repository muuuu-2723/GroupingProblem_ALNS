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
    int destroy_num;
public:
    /*�R���X�g���N�^*/
    Destroy(const std::vector<Item>& items, double init_weight, int param) : items(items), weight(init_weight ,param), destroy_num(2) {}
    /*����j��*/
    virtual std::vector<const Item*> operator()(Solution& solution) const { return std::vector<const Item*>(); }
    virtual void update_weight(double score) final;
    virtual double get_weight() const final;
    virtual void add_destroy_num(int add_num, const Solution& solution) { destroy_num += add_num; }
    virtual void set_destroy_num(int set_num, const Solution& solution) { destroy_num = set_num; }
    virtual int get_destroy_num() const final;
};

inline void Destroy::update_weight(double score) {
    weight.update(score);
}

inline double Destroy::get_weight() const {
    return weight.get_weight();
}

inline int Destroy::get_destroy_num() const {
    return destroy_num;
}

#include "RandomDestroy\RandomDestroy.hpp"
#include "RandomGroupDestroy\RandomGroupDestroy.hpp"
#include "MinimumDestroy\MinimumDestroy.hpp"
#include "MinimumGroupDestroy\MinimumGroupDestroy.hpp"
#include "UpperWeightGreedyDestroy\UpperWeightGreedyDestroy.hpp"

#endif