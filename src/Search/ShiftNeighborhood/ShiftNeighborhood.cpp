#include "ShiftNeighborhood.hpp"
#include <Search.hpp>
#include <Solution.hpp>
#include <MyRandom.hpp>
#include <Item.hpp>
#include <Group.hpp>
#include <Destroy.hpp>
#include <vector>
#include <numeric>
#include <cassert>
#include <typeinfo>
#include <memory>

using std::vector;

ShiftNeighborhood::ShiftNeighborhood(const std::vector<Item>& items, double init_weight, int param, const Solution& solution) : Search(items, init_weight, param, solution) {
    destructions.emplace_back(std::make_shared<Destroy>(items, 1, 1));
}

/*
 *shift近傍を探索
 *改善解があれば改善解を返す
 *なければ現在の解を返す
 *destroy_ptrがDestroy以外の場合, エラー
 */
std::unique_ptr<Solution> ShiftNeighborhood::operator()(const Solution& current_solution) {
    is_move = false;

    auto neighborhood_solution = std::make_unique<Solution>(current_solution);
    //std::cout << "shi_test" << std::endl;
    double max_diff = 0;
    int max_group_id;
    size_t max_item_id;

    //shift近傍内で最もよい解を探索
    for (auto&& item : items) {
        if (item.predefined_group != -1) continue;
        for (size_t g_id = 0; g_id < Group::N; ++g_id) {
            if (g_id != neighborhood_solution->get_group_id(item)) {
                //std::cerr << item.id << " " << g_id << std::endl;
                double diff_eval = neighborhood_solution->calc_diff_eval(neighborhood_solution->evaluation_shift(item, g_id));
                //std::cout << item.id << " " << g_id << " " << diff_eval << std::endl;
                if (diff_eval > max_diff) {
                    max_item_id = item.id;
                    max_group_id = g_id;
                    max_diff = diff_eval;
                }
            }
        }
    }
    //std::cout << max_diff << " " << max_group_id << " " << max_item_id << std::endl;

    if (max_diff != 0) {
        is_move = neighborhood_solution->shift_check(items[max_item_id], max_group_id);
    }

    return std::move(neighborhood_solution);
}