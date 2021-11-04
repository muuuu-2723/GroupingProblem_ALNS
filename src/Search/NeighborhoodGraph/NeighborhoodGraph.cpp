#include "NeighborhoodGraph.hpp"
#include <Search.hpp>
#include <Solution.hpp>
#include <Item.hpp>
#include <Group.hpp>
#include <Destroy.hpp>
#include <vector>
#include <cmath>
#include <cfloat>
#include <tuple>
#include <algorithm>
#include <memory>
#include <cassert>
#include <typeinfo>

using std::vector;

/*�R���X�g���N�^*/
NeighborhoodGraph::NeighborhoodGraph(const vector<Item>& items, double init_weight, int param) : Search(items,init_weight , param) {
    vertices.reserve(Item::N + Group::N);
    int idx = 0;
    for (auto&& item : items) {
        if (item.predefined_group == -1) {
            vertices.push_back(Vertex(idx++, item));
        }
    }
    dummy_items.reserve(Group::N);
    //�_�~�[�A�C�e���̐ݒ�. �A�C�e��id���A�C�e�����ȏ�ɐݒ�
    for (size_t i = 0; i < Group::N; ++i) {
        Item item;
        item.id = Item::N + i;
        dummy_items.push_back(item);
        vertices.push_back(Vertex(idx++, dummy_items[i]));
    }
}

/*�e���_�ԂɗL���ӂ�K�v�ɉ����Đݒ�*/
void NeighborhoodGraph::set_edge(Solution& solution) {
    graph.assign(vertices.size(), vector<Edge>());

    //s����t�ւ̗L���ӂ�ݒ�
    for (const auto& s : vertices) {
        for (const auto& t : vertices) {
            //�_�~�[�A�C�e�����m�ɂ͕ӂ𒣂�Ȃ�
            if (s.item.id >= Item::N && t.item.id >= Item::N) continue;

            int s_group_id = s.item.id < Item::N ? solution.get_group_id(s.item) : s.item.id - Item::N;
            int t_group_id = t.item.id < Item::N ? solution.get_group_id(t.item) : t.item.id - Item::N;

            //�����O���[�v�̃A�C�e���Ԃɂ͕ӂ𒣂�Ȃ�
            if (s_group_id != t_group_id) {
                const Group& t_group = solution.get_groups()[t_group_id];
                if (s.item.id >= Item::N) {                                                 //s���_�~�[�A�C�e���̏ꍇ
                    //t�����݂̃O���[�v����폜�����Ƃ��̃y�i���e�B���v�Z
                    double penalty = 0;
                    if (solution.get_eval_flags().test(Solution::EvalIdx::WEIGHT_PENA)) {
                        penalty += t_group.diff_weight_penalty({}, {&t.item});
                    }
                    if (solution.get_eval_flags().test(Solution::EvalIdx::GROUP_PENA)) {
                        penalty -= t.item.group_penalty[t_group_id];
                    }
                    if (solution.get_eval_flags().test(Solution::EvalIdx::ITEM_PENA)) {
                        penalty -= solution.get_each_group_item_penalty(t.item, t_group_id);
                    }

                    //�y�i���e�B���������Ȃ��ꍇ�ɕӂ𒣂�
                    if (penalty <= 0 || std::abs(penalty) < 1e-10) {
                        //�]���l�̕ω��ʂ��v�Z
                        double weight = solution.calc_diff_eval(solution.evaluation_diff({MoveItem(t.item, t_group_id, Group::N)}));

                        //�ӂ��O���t�ɒǉ�
                        graph[s.id].push_back(Edge(t.id, -weight));
                    }
                }
                else if (t.item.id >= Item::N) {                                            //t���_�~�[�A�C�e���̏ꍇ
                    //s��t_group�ɒǉ������Ƃ��̃y�i���e�B���v�Z
                    double penalty = 0;
                    if (solution.get_eval_flags().test(Solution::EvalIdx::WEIGHT_PENA)) {
                        penalty += t_group.diff_weight_penalty({&s.item}, {});
                    }
                    if (solution.get_eval_flags().test(Solution::EvalIdx::GROUP_PENA)) {
                        penalty += s.item.group_penalty[t_group_id];
                    }
                    if (solution.get_eval_flags().test(Solution::EvalIdx::ITEM_PENA)) {
                        penalty += solution.get_each_group_item_penalty(s.item, t_group_id);
                    }

                    //�y�i���e�B���������Ȃ��ꍇ�ɕӂ𒣂�
                    if (penalty <= 0 || std::abs(penalty) < 1e-10) {
                        //�]���l�̕ω��ʂ��v�Z
                        double weight = solution.calc_diff_eval(solution.evaluation_diff({MoveItem(s.item, Group::N, t_group_id)}));

                        //�ӂ��O���t�ɒǉ�
                        graph[s.id].push_back(Edge(t.id, -weight));
                    }
                }
                else {                                                                      //s��t���_�~�[�A�C�e���łȂ��ꍇ
                    
                    //t�����݂̃O���[�v����폜��, s�����̃O���[�v�ɒǉ������Ƃ��̃y�i���e�B���v�Z
                    double penalty = 0;
                    if (solution.get_eval_flags().test(Solution::EvalIdx::WEIGHT_PENA)) {
                        penalty += t_group.diff_weight_penalty({&s.item}, {&t.item});
                    }
                    if (solution.get_eval_flags().test(Solution::EvalIdx::GROUP_PENA)) {
                        penalty += s.item.group_penalty[t_group_id] - t.item.group_penalty[t_group_id];
                    }
                    if (solution.get_eval_flags().test(Solution::EvalIdx::ITEM_PENA)) {
                        penalty += solution.get_each_group_item_penalty(s.item, t_group_id) - solution.get_each_group_item_penalty(t.item, t_group_id) - s.item.item_penalty[t.item.id];
                    }
                    
                    //�y�i���e�B���������Ȃ��ꍇ�ɕӂ𒣂�
                    if (penalty <= 0 || std::abs(penalty) < 1e-10) {
                        //�]���l�̕ω��ʂ��v�Z
                        double weight = solution.calc_diff_eval(solution.evaluation_diff({MoveItem(s.item, Group::N, t_group_id), MoveItem(t.item, t_group_id, Group::N)}));

                        //�ӂ��O���t�ɒǉ�
                        graph[s.id].push_back(Edge(t.id, -weight));
                    }
                }
            }
        }
    }
}

/*
 *�O���t��T��
 *�O���t��̓����O���[�v��2��ʂ�Ȃ����H��T��
 *��L�̕��H�̈ړ���]���֐��ŕ]����, ���P���Ă���Έړ�����
 *destroy_ptr��Destroy�ȊO�̏ꍇ, �G���[
 */
std::unique_ptr<Solution> NeighborhoodGraph::operator()(const Solution& current_solution, std::shared_ptr<Destroy> destroy_ptr) {
    assert(typeid(*destroy_ptr) == typeid(Destroy));

    auto neighborhood_solution = std::make_unique<Solution>(current_solution);
    size_t valid_group_size = neighborhood_solution->get_valid_groups().size();
    std::cerr << "nei_test" << std::endl;
    set_edge(*neighborhood_solution);
    vector<vector<vector<double>>> dp(vertices.size(), vector<vector<double>>(vertices.size(), vector<double>(valid_group_size - 1, DBL_MAX)));
    using TablePos = std::tuple<int, int, int>;
    vector<vector<vector<TablePos>>> prev(vertices.size(), vector<vector<TablePos>>(vertices.size(), vector<TablePos>(valid_group_size - 1, {-1, -1, -1})));
    
    //�T���̌������̂��߂̃����_�֐�
    auto lambda = [](const double& a) { return a < 0 ? a : DBL_MAX; };
    
    //DP�ŕ��H��T��. ���H�̒����̓O���[�v���ȉ��̂��̂��T���̑Ώ�
    if (valid_group_size > 1) {
        for (const auto& v1 : vertices) {
            for (const auto& e : graph[v1.id]) {
                dp[v1.id][e.target][0] = lambda(e.weight);
            }

            for (size_t l = 1, size = valid_group_size - 1; l < size; ++l) {
                for (const auto& v2 : vertices) {
                    for (const auto& e : graph[v2.id]) {
                        if (dp[v1.id][v2.id][l - 1] != DBL_MAX && lambda(dp[v1.id][v2.id][l - 1] + e.weight) < dp[v1.id][e.target][l]) {
                            dp[v1.id][e.target][l] = lambda(dp[v1.id][v2.id][l - 1] + e.weight);
                            prev[v1.id][e.target][l] = {v1.id, v2.id, l - 1};
                        }
                    }
                }
            }
        }
    }
    

    //DP�e�[�u�����畉�H���ɏd�݂̍��v�Ǝn�_�𒊏o
    vector<std::pair<double, TablePos>> start_pos;
    for (const auto& v : vertices) {
        for (const auto& e : graph[v.id]) {
            for (size_t l = 0, size = valid_group_size - 1; l < size; ++l) {
                if (dp[e.target][v.id][l] + e.weight < 0) {
                    start_pos.push_back(std::make_pair(dp[e.target][v.id][l] + e.weight, std::make_tuple(e.target, v.id, l)));
                }
            }
        }
    }
    
    //�d�݂ŕ��H���\�[�g
    std::sort(start_pos.begin(), start_pos.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    //���H���\�z��, �����O���[�v��2��ȏ�ʂ��Ă��Ȃ�������
    for (const auto& sp : start_pos) {
        //���H�\�z
        vector<int> cycle;
        auto [v1, v2, l] = sp.second;
        cycle.reserve(l + 2);
        cycle.push_back(v1);
        auto pos = sp.second;
        while (std::get<0>(pos) != -1) {
            auto [now_v1, now_v2, now_l] = pos;
            cycle.push_back(now_v2);
            pos = prev[now_v1][now_v2][now_l];
        }
        cycle.push_back(v1);

        vector<MoveItem> move_items;
        move_items.reserve(l + 1);
        bool is_duplicated = false;                     //�����O���[�v��2��ȏ�ʂ��Ă��邩��\��bool�^
        unsigned int flag = 0;                          //�O���[�v��ʂ��������Ǘ�����r�b�g�t���O

        //�����O���[�v��2��ȏ�ʂ��Ă��Ȃ�������
        for (auto ritr = cycle.rbegin(), rend = --cycle.rend(); ritr != rend; ++ritr) {
            const Item& item = vertices[*ritr].item;
            int now_group_id;
            if (item.id < Item::N) {
                now_group_id = neighborhood_solution->get_group_id(item);
                const Item& next_item = vertices[*std::next(ritr)].item;
                int next_group_id;
                if (next_item.id < Item::N) {
                    next_group_id = neighborhood_solution->get_group_id(next_item);
                }
                else {
                    next_group_id = next_item.id - Item::N;
                }
                move_items.push_back(MoveItem(item, now_group_id, next_group_id));
            }
            else {
                now_group_id = item.id - Item::N;
            }

            if (!(flag & (1<<now_group_id))) {              //item�̃O���[�v�Ƀt���O�������Ă��Ȃ��ꍇ
                //�t���O�𗧂Ă�
                flag |= (1<<now_group_id);
            }
            else {                                          //item�̃O���[�v�ɂ��łɃt���O�������Ă���ꍇ
                is_duplicated = true;
                break;
            }
        }
        /*if (is_duplicated) continue;

        //���H�̈ړ������P���ɂȂ��Ă���ꍇ, �ړ����ă��[�v�𔲂���
        if (neighborhood_solution->move_check(move_items)) break;*/
        if (!is_duplicated) {
            std::cout << move_items.size() << std::endl;
            neighborhood_solution->move(move_items);
            break;
        }
    }
    return std::move(neighborhood_solution);
}