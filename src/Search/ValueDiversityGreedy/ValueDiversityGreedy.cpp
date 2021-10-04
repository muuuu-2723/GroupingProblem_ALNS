#include "ValueDiversityGreedy.hpp"
#include <Solution.hpp>
#include <Group.hpp>
#include <Destroy.hpp>
#include <Item.hpp>
#include <MyRandom.hpp>
#include <memory>
#include <vector>
#include <cassert>
#include <typeinfo>
#include <algorithm>
#include <numeric>
#include <unordered_map>

using std::vector;

Solution ValueDiversityGreedy::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    assert(typeid(*destroy_ptr) == typeid(RandomGroupDestroy) || typeid(*destroy_ptr) == typeid(MinimumGroupDestroy));
    std::unique_ptr<Solution> best;

    if (Item::v_size < 2) {
        for (size_t i = 0; i < 30; ++i) {
            std::unique_ptr<Solution> neighborhood(new Solution(current_solution));
            (*destroy_ptr)(*neighborhood);

            vector<int> search_group_ids;
            search_group_ids.reserve(Group::N);
            auto [current_begin, current_end] = current_solution.get_groups_range();
            auto [neighborhood_begin, neighborhood_end] = neighborhood->get_groups_range();

            for (auto citr1 = current_begin, citr2 = neighborhood_begin;
                 citr1 != current_end || citr2 != neighborhood_end; ++citr1, ++citr2) 
                    if (citr1->get_member_num() != citr2->get_member_num()) search_group_ids.push_back(citr1->get_id());

            auto& member_list = neighborhood->get_dummy_group().get_member_list();
            vector<int> value_sort_ids(member_list.begin(), member_list.end());

            std::sort(value_sort_ids.begin(), value_sort_ids.end(), [&](const auto& a, const auto& b) { return items[a].values[0] > items[b].values[0]; });

            vector<vector<int>> blocks(value_sort_ids.size() / search_group_ids.size() + 1);
            for (size_t j = 0, size = value_sort_ids.size(); j < size; ++j) {
                blocks[j / search_group_ids.size()].push_back(value_sort_ids[j]);
            }

            for (auto&& block : blocks) {
                MyRandom::shuffle(block);
            }

            vector<MoveItem> move_items;
            move_items.reserve(value_sort_ids.size());
            for (const auto& block : blocks) {
                for (size_t j = 0, size = block.size(); j < size; ++j) {
                    move_items.push_back(MoveItem(items[block[j]], neighborhood->get_group_id(items[block[j]]), search_group_ids[j]));
                }
            }

            neighborhood->move(move_items);

            if (!best || best->get_eval_value() < neighborhood->get_eval_value()) {
                best = std::move(neighborhood);
            }
        }
    }
    else {
        vector<int> value_type(Item::v_size);
        std::iota(value_type.begin(), value_type.end(), 0);

        for (size_t i = 0; i < 30; ++i) {
            MyRandom::shuffle(value_type);
            std::unique_ptr<Solution> neighborhood(new Solution(current_solution));
            (*destroy_ptr)(*neighborhood);

            vector<int> search_group_ids;
            search_group_ids.reserve(Group::N);
            auto [current_begin, current_end] = current_solution.get_groups_range();
            auto [neighborhood_begin, neighborhood_end] = neighborhood->get_groups_range();

            for (auto citr1 = current_begin, citr2 = neighborhood_begin;
                 citr1 != current_end || citr2 != neighborhood_end; ++citr1, ++citr2) 
                    if (citr1->get_member_num() != citr2->get_member_num()) search_group_ids.push_back(citr1->get_id());

            auto& member_list = neighborhood->get_dummy_group().get_member_list();
            vector<vector<int>> value_sort_ids(2, vector<int>(member_list.begin(), member_list.end()));
            
            for (size_t j = 0; j < 2; ++j) {
                std::sort(value_sort_ids[j].begin(), value_sort_ids[j].end(), [&](const auto& a, const auto& b) { return items[a].values[value_type[j]] < items[b].values[value_type[j]]; });
            }
            
            vector<vector<vector<int>>> blocks(2, vector<vector<int>>(value_sort_ids[0].size() / search_group_ids.size() + 1));
            std::unordered_map<int, int> block_id;
            for (size_t j = 0; j < 2; ++j) {
                for (size_t k = 0, size = search_group_ids.size(); k < size; ++k) {
                    blocks[j][k / search_group_ids.size()].push_back(value_sort_ids[j][k]);
                    if (j == 1) block_id[value_sort_ids[j][k]] = k / search_group_ids.size();
                }
            }

            vector<MoveItem> move_items;
            move_items.reserve(value_sort_ids.size());
            vector<vector<int>> assignment(blocks.size(), vector<int>(blocks[0].size(), 0));
            for (auto&& block : blocks[0]) {
                std::sort(block.begin(), block.end(), [&](const auto& a, const auto& b) {
                    std::accumulate(assignment[block_id[a]].begin(), assignment[block_id[a]].end(), 0) < std::accumulate(assignment[block_id[b]].begin(), assignment[block_id[b]].end(), 0);
                });
                for (auto&& id : block) {
                    int idx = 0;
                    while (assignment[block_id[id]][idx] != 0) ++idx;
                    move_items.push_back(MoveItem(items[id], neighborhood->get_group_id(items[id]), search_group_ids[idx]));
                    assignment[block_id[id]][idx] = 1;
                }
            }

            neighborhood->move(move_items);
            
            if (!best || best->get_eval_value() < neighborhood->get_eval_value()) {
                best = std::move(neighborhood);
            }
        }
    }
    
    return std::move(*best);
}