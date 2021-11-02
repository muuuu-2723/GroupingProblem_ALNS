#include "WeightPenaltyGreedy.hpp"
#include <Solution.hpp>
#include <Group.hpp>
#include <Destroy.hpp>
#include <Item.hpp>
#include <MyRandom.hpp>
#include <memory>
#include <vector>
#include <climits>
#include <iostream>
#include <list>
#include <typeinfo>
#include <numeric>
#include <utility>
#include <algorithm>

using std::vector;

/*
 *�×~�@�ŐV���ȉ��𐶐�
 *destroy_ptr�ŉ���j��, �����ɑ���Ă��Ȃ��O���[�v�ɗD�悵�ăA�C�e�������蓖�Ă�
 *���̌�, ����𒴂��Ȃ��O���[�v�Ɋ��蓖�ĂĂ���
 *�֌W����A�C�e��(weight[type]!=0)�����Ȃ�type��weight��D�悷��
 */
std::unique_ptr<Solution> WeightPenaltyGreedy::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    std::unique_ptr<Solution> best;                                                 //�����������ň�ԗǂ��]���l�̉�
    size_t N = 20;

    //�����_���j��łȂ��ꍇ, �J��Ԃ��K�v���Ȃ�����N = 1�ɂ���
    if (typeid(*destroy_ptr) == typeid(MinimumDestroy) || typeid(*destroy_ptr) == typeid(MinimumGroupDestroy)) {
        N = 1;
    }
    std::cout << "wp_test" << std::endl;
    //std::cout << current_solution << std::endl;

    for (size_t i = 0; i < N; ++i) {
        //���݂̉����R�s�[��, �����j��
        auto neighborhood = std::make_unique<Solution>(current_solution);
        (*destroy_ptr)(*neighborhood);

        const Group& dummy_group = neighborhood->get_dummy_group();
        vector<MoveItem> move_items;
        move_items.reserve(dummy_group.get_member_num());
        //���łɊ��蓖�Ă��A�C�e�����폜���Ă������߃R�s�[���Ƃ�
        auto member_list = dummy_group.get_member_list();

        //type���֌W����A�C�e��(weight[type]!=0)�����Ȃ����ɂ���
        vector<std::pair<int, int>> priority_type(Item::w_size);
        for (size_t j = 0; j < Item::w_size; ++j) {
            priority_type[j].first = j;
            int cnt = 0;
            for (const auto& id : member_list) {
                if (items[id].weight[j] == 0) ++cnt;
            }
            priority_type[j].second = cnt;
        }
        std::sort(priority_type.begin(), priority_type.end(), [](const auto& a, const auto& b) { return a.second > b.second; });
        //�����ɑ���Ă��Ȃ��O���[�v�ɗD�悵�ăA�C�e�������蓖�Ă�
        for (auto [type, cnt] : priority_type) {
            for (auto m_itr = member_list.begin(); m_itr != member_list.end();) {
                const Item& item = items[*m_itr];
                bool is_move = false;
                auto [group_begin, group_end] = neighborhood->get_groups_range();
                for (auto g_itr = group_begin; g_itr != group_end; ++g_itr) {
                    auto& lower = g_itr->get_lower();
                    if (item.weight[type] != 0 && g_itr->get_sum_weight()[type] < lower[type]) {
                        neighborhood->move({MoveItem(item, neighborhood->get_group_id(item), g_itr->get_id())});
                        m_itr = member_list.erase(m_itr);
                        is_move = true;
                        break;
                    }
                }
                if (!is_move) ++m_itr;
            }
        }
        //�c��̃A�C�e��������𒴂��Ȃ��O���[�v�Ɋ��蓖�Ă�
        auto [group_begin, group_end] = neighborhood->get_groups_range();
        vector<size_t> shuffle_group_ids(std::distance(group_begin, group_end));
        std::iota(shuffle_group_ids.begin(), shuffle_group_ids.end(), 0);
        MyRandom::shuffle(shuffle_group_ids);
        vector<vector<const Item*>> add_members(Group::N);
        for (const auto& id : member_list) {
            int min_group_id = -1;
            int min_penalty = INT_MAX;
            for (auto&& id : shuffle_group_ids) {
                auto g_itr = group_begin + id;
                vector<const Item*> tmp = add_members[g_itr->get_id()];
                tmp.push_back(&items[id]);
                int diff_penalty = 0;
                if (neighborhood->get_eval_flags().test(Solution::EvalIdx::WEIGHT_PENA)) {
                    diff_penalty += g_itr->diff_weight_penalty(tmp, {}) - g_itr->diff_weight_penalty(add_members[g_itr->get_id()], {});
                }
                if (diff_penalty < min_penalty) {
                    min_penalty = diff_penalty;
                    min_group_id = g_itr->get_id();
                }
            }
            move_items.push_back(MoveItem(items[id], neighborhood->get_group_id(items[id]), min_group_id));
            add_members[min_group_id].push_back(&items[id]);
        }
        neighborhood->move(move_items);
        if (!best || best->get_eval_value() < neighborhood->get_eval_value()) {
            best = std::move(neighborhood);
        }
    }
    //std::cout << *best << std::endl;
    return std::move(best);
}