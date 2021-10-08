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

/*
 *shift‹ß–T‚ğ’Tõ
 *‰ü‘P‰ğ‚ª‚ ‚ê‚Î‰ü‘P‰ğ‚ğ•Ô‚·
 *‚È‚¯‚ê‚ÎŒ»İ‚Ì‰ğ‚ğ•Ô‚·
 *destroy_ptr‚ªDestroyˆÈŠO‚Ìê‡, ƒGƒ‰[
 */
Solution ShiftNeighborhood::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    assert(typeid(*destroy_ptr) == typeid(Destroy));

    Solution neighborhood_solution(current_solution);
    double max_diff = 0;
    int max_group_id;
    std::unique_ptr<const Item> max_item_ptr;

    //shift‹ß–T“à‚ÅÅ‚à‚æ‚¢‰ğ‚ğ’Tõ
    for (const auto& item : items) {
        if (item.predefined_group != -1) continue;
        for (size_t g_id = 0; g_id < Group::N; ++g_id) {
            if (g_id != neighborhood_solution.get_group_id(item)) {
                auto [diff_r, diff_p, diff_ave, diff_sum] = neighborhood_solution.evaluation_shift(item, g_id);
                double diff_eval = diff_r * neighborhood_solution.get_relation_parameter() - diff_p * neighborhood_solution.get_penalty_parameter() - diff_ave * neighborhood_solution.get_ave_balance_parameter() + diff_sum * neighborhood_solution.get_sum_balance_parameter();
                if (diff_eval > max_diff) {
                    max_item_ptr.reset(&item);
                    max_group_id = g_id;
                    max_diff = diff_eval;
                }
            }
        }
    }

    if (max_diff != 0) {
        neighborhood_solution.shift_check(*max_item_ptr, max_group_id);
    }

    return neighborhood_solution;
}