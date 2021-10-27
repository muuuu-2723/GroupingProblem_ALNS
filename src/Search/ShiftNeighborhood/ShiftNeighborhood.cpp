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
std::unique_ptr<Solution> ShiftNeighborhood::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    assert(typeid(*destroy_ptr) == typeid(Destroy));

    auto neighborhood_solution = std::make_unique<Solution>(current_solution);
    std::cout << "shi_test" << std::endl;
    double max_diff = -DBL_MAX;
    int max_group_id;
    size_t max_item_id;

    //shift‹ß–T“à‚ÅÅ‚à‚æ‚¢‰ğ‚ğ’Tõ
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
    std::cout << max_diff << " " << max_group_id << " " << max_item_id << std::endl;

    if (max_diff != -DBL_MAX) {
        neighborhood_solution->shift_move(items[max_item_id], max_group_id);
    }

    return std::move(neighborhood_solution);
}