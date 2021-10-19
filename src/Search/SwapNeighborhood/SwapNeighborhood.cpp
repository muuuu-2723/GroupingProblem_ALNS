#include "SwapNeighborhood.hpp"
#include <Search.hpp>
#include <Solution.hpp>
#include <MyRandom.hpp>
#include <Item.hpp>
#include <Group.hpp>
#include <Destroy.hpp>
#include <vector>
#include <algorithm>
#include <cassert>
#include <typeinfo>
#include <memory>

using std::vector;

/*
 *swap近傍を探索
 *改善解があれば改善解を返す
 *なければ現在の解を返す
 *destroy_ptrがDestroy以外の場合, エラー
 */
Solution SwapNeighborhood::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    assert(typeid(*destroy_ptr) == typeid(Destroy));

    Solution neighborhood_solution(current_solution);
    double max_diff = 0;
    std::unique_ptr<const Item> max_item1_ptr, max_item2_ptr;

    //swap近傍内で最も良い解を探索
    for (auto item1_itr = items.begin(), end = items.end(); item1_itr != end; ++item1_itr) {
        if (item1_itr->predefined_group != -1) continue;
        for (auto item2_itr = std::next(item1_itr); item2_itr != end; ++item2_itr) {
            if (item2_itr->predefined_group != -1) continue;
            if (neighborhood_solution.get_group_id(*item1_itr) != neighborhood_solution.get_group_id(*item2_itr)) {
                double diff_eval = neighborhood_solution.calc_diff_eval(neighborhood_solution.evaluation_swap(*item1_itr, *item2_itr));
                if (diff_eval > max_diff) {
                    max_item1_ptr.reset(&(*item1_itr));
                    max_item2_ptr.reset(&(*item2_itr));
                    max_diff = diff_eval;
                }
            }
        }
    }

    if (max_diff != 0) {
        neighborhood_solution.swap_check(*max_item1_ptr, *max_item2_ptr);
    }
    return neighborhood_solution;
}