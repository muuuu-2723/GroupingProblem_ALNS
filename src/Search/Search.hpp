#ifndef SEARCH_H_
#define SEARCH_H_

#include <Weight.hpp>
#include <Destroy.hpp>
#include <MyRandom.hpp>
#include <vector>
#include <memory>
#include <algorithm>

struct Item;
class Solution;

/*近傍と構築法の基底クラス*/
class Search {
protected:
    const std::vector<Item>& items;                             //グループ分けするアイテムの集合
    Weight weight;                                              //選択確率を決める重み
    bool is_move;
    int init_item_destroy_num;
    int init_group_destroy_num;
    std::vector<std::shared_ptr<Destroy>> destructions;
    std::vector<std::shared_ptr<Destroy>> item_destroy;
    std::vector<std::shared_ptr<Destroy>> group_destroy;
    std::shared_ptr<Destroy> last_use_destroy;
    std::unique_ptr<DiscreteDistribution> destroy_random;
    virtual void init_destroy_random() final;
public:
    /*コンストラクタ*/
    Search(const std::vector<Item>& items, double init_weight, int param, const Solution& solution)
     : items(items), weight(init_weight, param), is_move(false) {
        init_item_destroy_num = (0.5 * Item::N) / Group::N;
        init_group_destroy_num = solution.get_valid_groups().size() / 5;
    }
    /*新たな解を生成*/
    virtual std::unique_ptr<Solution> operator()(const Solution& current_solution) = 0;
    virtual void reset_destroy_num(const Solution& solution);
    virtual void update_destroy_num(const Solution& solution, bool intensification);
    virtual void update_weight(double score);
    virtual double get_weight() const final;
    virtual bool get_is_move() const final;
    virtual const Destroy& select_destroy();
    virtual const std::unique_ptr<DiscreteDistribution>& get_destroy_random() const;
};

inline void Search::init_destroy_random() {
    auto itr = destructions.begin();
    std::vector<double> weights(destructions.size());
    std::generate(weights.begin(), weights.end(), [&itr]() { return (*itr++)->get_weight(); });
    destroy_random = std::make_unique<DiscreteDistribution>(weights);
}

inline void Search::reset_destroy_num(const Solution& solution) {
    for (auto&& d : item_destroy) {
        d->set_destroy_num(3, solution);
    }
    for (auto&& d : group_destroy) {
        d->set_destroy_num(2, solution);
    }
}

inline void Search::update_destroy_num(const Solution& solution, bool intensification) {
    int add_num;
    if (intensification) {
        add_num = -1;
    }
    else {
        add_num = 1;
    }
    for (auto&& d : item_destroy) {
        d->add_destroy_num(add_num, solution);
    }
    if (item_destroy.size() > 0) std::cerr << "item:" << item_destroy[0]->get_destroy_num() << ", ";
    int group_destroy_num = (int)(item_destroy[0]->get_destroy_num() / ((double)Item::N / solution.get_valid_groups().size()));
    for (auto&& d : group_destroy) {
        d->set_destroy_num(group_destroy_num, solution);
    }
    std::cerr << "group:" << group_destroy[0]->get_destroy_num() << std::endl;
}

/*重みの更新*/
inline void Search::update_weight(double score) {
    weight.update(score);
    last_use_destroy->update_weight(score);
    std::vector<double> weights(destructions.size());
    auto itr = destructions.begin();
    std::generate(weights.begin(), weights.end(), [&itr]() { return (*itr++)->get_weight(); });
    destroy_random->set_weight(weights);
}

/*重みの取得*/
inline double Search::get_weight() const {
    return weight.get_weight();
}

inline bool Search::get_is_move() const {
    return is_move;
}

inline const Destroy& Search::select_destroy() {
    last_use_destroy = destructions[(*destroy_random)()];
    return *last_use_destroy;
}

inline const std::unique_ptr<DiscreteDistribution>& Search::get_destroy_random() const {
    return destroy_random;
}

#include "NeighborhoodGraph/NeighborhoodGraph.hpp"
#include "ShiftNeighborhood/ShiftNeighborhood.hpp"
#include "SwapNeighborhood/SwapNeighborhood.hpp"
#include "GroupPenaltyGreedy/GroupPenaltyGreedy.hpp"
#include "ItemPenaltyGreedy/ItemPenaltyGreedy.hpp"
#include "WeightPenaltyGreedy/WeightPenaltyGreedy.hpp"
#include "RelationGreedy/RelationGreedy.hpp"
#include "ValueAverageGreedy/ValueAverageGreedy.hpp"
#include "ValueDiversityGreedy/ValueDiversityGreedy.hpp"
#include "ValueSumGreedy/ValueSumGreedy.hpp"
#include "DecreaseGroup/DecreaseGroup.hpp"

#endif