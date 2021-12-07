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
#include <set>

using std::vector;

ValueDiversityGreedy::ValueDiversityGreedy(const vector<Item>& items, double init_weight, int param, const Solution& solution) : Search(items, init_weight, param, solution) {
    value_types.resize(Item::v_size);
    std::iota(value_types.begin(), value_types.end(), 0);

    destructions.emplace_back(std::make_shared<RandomGroupDestroy>(items, init_group_destroy_num, 1, 1, solution));
    destructions.emplace_back(std::make_shared<MinimumGroupDestroy>(items, init_group_destroy_num, 1, 1, solution));
    destructions.emplace_back(std::make_shared<UpperWeightGreedyDestroy>(items, init_group_destroy_num, 1, 1, solution));
    
    init_destroy_random();

    for (auto&& d : destructions) {
        group_destroy.emplace_back(d);
    }
    item_destroy.emplace_back(std::make_shared<Destroy>(items, 1, 1));
}

/*
 *�×~�@�ŐV���ȉ��𐶐�
 *destroy_ptr�ŉ���j��, value�Ń\�[�g���ď��Ɋ��蓖�Ă�
 *2��ނ�value�܂őΉ���, ����ȏ�̎�ނ�����ꍇ�̓����_����2��ނ�I��
 *MinimumGroupDestroy��RandomGroupDestroy�ȊO�̔j��@�̏ꍇ�̓G���[
 */
std::unique_ptr<Solution> ValueDiversityGreedy::operator()(const Solution& current_solution) {
    std::unique_ptr<Solution> best;                                                     //�����������ň�ԗǂ��]���l�̉�
    //std::cout << "vd_test" << std::endl;
    //std::cout << current_solution << std::endl;
    auto& destroy = select_destroy();
    if (Item::v_size < 2) {                                                             //value�̎�ނ�0 or 1��ނ̎�
        for (size_t i = 0; i < /*30*/10; ++i) {
            //���݂̉����R�s�[��, �����j��
            auto neighborhood = std::make_unique<Solution>(current_solution);
            auto destroy_items = destroy(*neighborhood);

            std::set<int> search_group_ids;

            for (auto&& item : destroy_items) {
                search_group_ids.insert(neighborhood->get_group_id(*item));
            }

            vector<MoveItem> destroy_move;
            destroy_move.reserve(destroy_items.size());
            for (auto&& item : destroy_items) {
                destroy_move.push_back(MoveItem(*item, neighborhood->get_group_id(*item), neighborhood->get_dummy_group().get_id()));
            }
            neighborhood->move(destroy_move);

            auto& member_list = neighborhood->get_dummy_group().get_member_list();
            vector<int> value_sort_ids(member_list.begin(), member_list.end());

            //�j�󂳂ꂽ�A�C�e����value���ɕ��בւ���
            std::sort(value_sort_ids.begin(), value_sort_ids.end(), [&](const auto& a, const auto& b) { return items[a].values[0] > items[b].values[0]; });

            //�j�󂳂ꂽ�O���[�v�����Ƃ̃u���b�N�ɃA�C�e���𕪂���
            //���̃u���b�N�������l���O���[�v�ɐU�蕪����
            vector<vector<int>> blocks(value_sort_ids.size() / search_group_ids.size() + 1);
            for (size_t j = 0, size = value_sort_ids.size(); j < size; ++j) {
                blocks[j / search_group_ids.size()].push_back(value_sort_ids[j]);
            }

            //�u���b�N�����V���b�t��
            for (auto&& block : blocks) {
                MyRandom::shuffle(block);
            }

            //�e�O���[�v�Ɋ��蓖��
            vector<MoveItem> move_items;
            move_items.reserve(value_sort_ids.size());
            for (auto begin = search_group_ids.begin(), itr = begin, end = search_group_ids.end(); itr != end; ++itr) {
                int j = std::distance(begin, itr);
                for (const auto& block : blocks) {
                    if (block.size() <= j) break;
                    move_items.push_back(MoveItem(items[block[j]], neighborhood->get_group_id(items[block[j]]), *itr));
                }
            }

            neighborhood->move(move_items);

            if (!best || best->get_eval_value() < neighborhood->get_eval_value()) {
                best = std::move(neighborhood);
                is_move = (move_items.size() > 0);
            }
        }
    }
    else {                                                                              //value�̎�ނ�2��ވȏ�̎�
        vector<int> value_type(Item::v_size);
        std::iota(value_type.begin(), value_type.end(), 0);

        for (size_t i = 0; i < /*30*/10; ++i) {
            //�����_����2��ނ�I��
            MyRandom::shuffle(value_type);

            //���݂̉����R�s�[��, �����j��
            auto neighborhood = std::make_unique<Solution>(current_solution);
            auto destroy_items = destroy(*neighborhood);

            std::set<int> search_group_ids;

            for (auto&& item : destroy_items) {
                search_group_ids.insert(neighborhood->get_group_id(*item));
            }

            vector<MoveItem> destroy_move;
            destroy_move.reserve(destroy_items.size());
            for (auto&& item : destroy_items) {
                destroy_move.push_back(MoveItem(*item, neighborhood->get_group_id(*item), neighborhood->get_dummy_group().get_id()));
            }
            neighborhood->move(destroy_move);

            auto& member_list = neighborhood->get_dummy_group().get_member_list();
            vector<vector<int>> value_sort_ids(2, vector<int>(member_list.begin(), member_list.end()));

            //�I�΂ꂽ2��ނ�value���ƂɃ\�[�g����
            for (size_t j = 0; j < 2; ++j) {
                std::sort(value_sort_ids[j].begin(), value_sort_ids[j].end(), [&](const auto& a, const auto& b) { return items[a].values[value_type[j]] < items[b].values[value_type[j]]; });
            }
            
            //�j�󂳂ꂽ�O���[�v�����Ƃ̃u���b�N�ɃA�C�e���𕪂���
            vector<vector<vector<int>>> blocks(2, vector<vector<int>>(value_sort_ids[0].size() / search_group_ids.size() + 1));
            std::unordered_map<int, int> block_id;
            for (size_t j = 0; j < 2; ++j) {
                for (size_t k = 0, size = value_sort_ids[j].size(); k < size; ++k) {
                    blocks[j][k / search_group_ids.size()].push_back(value_sort_ids[j][k]);
                    if (j == 1) block_id[value_sort_ids[j][k]] = k / search_group_ids.size();
                }
            }

            //�u���b�N���Ŋ��蓖�Ă���ꏊ�����Ȃ��A�C�e����D�悵�Ċ��蓖�Ă�
            vector<MoveItem> move_items;
            move_items.reserve(value_sort_ids.size());
            vector<vector<int>> assignment(blocks[0].size(), vector<int>(blocks[0][0].size(), 0));
            for (auto&& block : blocks[0]) {
                auto begin = search_group_ids.begin();
                std::sort(block.begin(), block.end(), [&](const auto& a, const auto& b) {
                    return std::accumulate(assignment[block_id[a]].begin(), assignment[block_id[a]].end(), 0) < std::accumulate(assignment[block_id[b]].begin(), assignment[block_id[b]].end(), 0);
                });
                for (auto&& id : block) {
                    auto itr = begin;
                    while (assignment[block_id[id]][std::distance(begin, itr)] != 0) ++itr;
                    move_items.push_back(MoveItem(items[id], neighborhood->get_group_id(items[id]), *itr));
                    assignment[block_id[id]][std::distance(begin, itr)] = 1;
                }
            }

            neighborhood->move(move_items);
            
            if (!best || best->get_eval_value() < neighborhood->get_eval_value()) {
                best = std::move(neighborhood);
                is_move = (move_items.size() > 0);
            }
        }
    }
    //std::cout << *best << std::endl;
    return std::move(best);
}