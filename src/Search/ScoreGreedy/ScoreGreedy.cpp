#include "ScoreGreedy.hpp"
#include <Solution.hpp>
#include <Group.hpp>
#include <Destroy.hpp>
#include <Person.hpp>
#include <MyRandom.hpp>
#include <memory>
#include <vector>
#include <cassert>
#include <typeinfo>
#include <tuple>
#include <algorithm>
#include <functional>

using std::vector;

Solution ScoreGreedy::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    assert(typeid(*destroy_ptr) == typeid(RandomGroupDestroy) || typeid(*destroy_ptr) == typeid(MinimumGroupDestroy));
    std::unique_ptr<Solution> best;
    
    for (int i = 0; i < /*40*/30; ++i) {
        std::unique_ptr<Solution> neighborhood_solution(new Solution(current_solution));
        (*destroy_ptr)(*neighborhood_solution);

        vector<int> search_gorup_ids;
        search_gorup_ids.reserve(Group::N);
        auto [current_begin, current_end] = current_solution.get_groups_range();
        auto [neighborhood_begin, neighborhood_end] = neighborhood_solution->get_groups_range();

        for (auto citr1 = current_begin, citr2 = neighborhood_begin;
             citr1 != current_end || citr2 != neighborhood_end; ++citr1, ++citr2) 
                if (citr1->get_member_num() != citr2->get_member_num()) search_gorup_ids.push_back(citr1->get_id());
        
        const auto& member_list = neighborhood_solution->get_dummy_group().get_member_list();
        vector<int> score_sort_ids(member_list.begin(), member_list.end());

        std::sort(score_sort_ids.begin(), score_sort_ids.end(), [&](const auto& a, const auto& b) { return persons[a].score > persons[b].score; });
        
        vector<vector<int>> blocks(score_sort_ids.size() / search_gorup_ids.size() + 1);
        for (int j = 0, size = score_sort_ids.size(); j < size; ++j) {
            blocks[j / search_gorup_ids.size()].push_back(score_sort_ids[j]);
        }

        //RandomInt<> rnd(0, 1);
        for (auto&& block : blocks) {
            /*if (rnd() == 1) {
                std::reverse(block.begin(), block.end());
            }*/
            MyRandom::shuffle(block);
        }

        vector<MovePerson> move_persons;
        move_persons.reserve(score_sort_ids.size());
        for (const auto& block : blocks) {
            for (int j = 0, size = block.size(); j < size; ++j) {
                move_persons.push_back(MovePerson(persons[block[j]], neighborhood_solution->get_group_id(persons[block[j]]), search_gorup_ids[j]));
            }
        }

        neighborhood_solution->move(move_persons);
        
        if (!best || best->get_eval_value() < neighborhood_solution->get_eval_value()) {
            best = std::move(neighborhood_solution);
        }
    }
    return std::move(*best);
}