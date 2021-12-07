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

SwapNeighborhood::SwapNeighborhood(const std::vector<Item>& items, double init_weight, int param, const Solution& solution) : Search(items, init_weight, param, solution) {
    destructions.emplace_back(std::make_shared<Destroy>(items, 1, 1));
}

/*
 *swap‹ß–T‚ğ’Tõ
 *‰ü‘P‰ğ‚ª‚ ‚ê‚Î‰ü‘P‰ğ‚ğ•Ô‚·
 *‚È‚¯‚ê‚ÎŒ»İ‚Ì‰ğ‚ğ•Ô‚·
 *destroy_ptr‚ªDestroyˆÈŠO‚Ìê‡, ƒGƒ‰[
 */
std::unique_ptr<Solution> SwapNeighborhood::operator()(const Solution& current_solution) {
    is_move = false;

    auto neighborhood_solution = std::make_unique<Solution>(current_solution);
    //std::cout << "swa_test" << std::endl;
    double max_diff = 0;
    size_t max_item1_id, max_item2_id;

    //swap‹ß–T“à‚ÅÅ‚à—Ç‚¢‰ğ‚ğ’Tõ
    for (auto item1_itr = items.begin(), end = items.end(); item1_itr != end; ++item1_itr) {
        if (item1_itr->predefined_group != -1) continue;
        for (auto item2_itr = std::next(item1_itr); item2_itr != end; ++item2_itr) {
            if (item2_itr->predefined_group != -1) continue;
            if (neighborhood_solution->get_group_id(*item1_itr) != neighborhood_solution->get_group_id(*item2_itr)) {
                //std::cout << item1_itr->id << " " << item2_itr->id << std::endl;
                double diff_eval = neighborhood_solution->calc_diff_eval(neighborhood_solution->evaluation_swap(*item1_itr, *item2_itr));
                if (diff_eval > max_diff) {
                    max_item1_id = item1_itr->id;
                    max_item2_id = item2_itr->id;
                    max_diff = diff_eval;
                }
            }
        }
    }
    if (max_diff != 0) {
        is_move = neighborhood_solution->swap_check(items[max_item1_id], items[max_item2_id]);
    }
    return std::move(neighborhood_solution);
}