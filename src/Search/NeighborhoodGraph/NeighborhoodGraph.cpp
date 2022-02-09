#include "NeighborhoodGraph.hpp"
#include <Search.hpp>
#include <Solution.hpp>
#include <Item.hpp>
#include <Group.hpp>
#include <Destroy.hpp>
#include <MyRandom.hpp>
#include <vector>
#include <cmath>
#include <cfloat>
#include <tuple>
#include <algorithm>
#include <memory>
#include <cassert>
#include <typeinfo>

//���񉻃I�v�V�������L���̂Ƃ�
#ifdef _OPENMP
#include <omp.h>
#endif

using std::vector;

/*�R���X�g���N�^*/
NeighborhoodGraph::NeighborhoodGraph(const vector<Item>& items, double init_weight, int param, const Solution& solution) : Search(items,init_weight , param, solution) {
    destroy_ratio = 1;
    if (items.size() + Group::N >= 550) {
        destroy_ratio = 550.0 / (items.size() + Group::N) < 0.4 ? 0.4 : 550.0 / (items.size() + Group::N);
    }
    destructions.emplace_back(std::make_shared<RandomDestroy>(items, items.size(), 50, 1, solution, destroy_ratio));
    destructions.emplace_back(std::make_shared<RandomGroupDestroy>(items, solution.get_valid_groups().size(), 50, 1, solution, destroy_ratio));
    destructions.emplace_back(std::make_shared<MinimumDestroy>(items, items.size(), 50, 1, solution, destroy_ratio));
    destructions.emplace_back(std::make_shared<MinimumGroupDestroy>(items, solution.get_valid_groups().size(), 50, 1, solution, destroy_ratio));
    destructions.emplace_back(std::make_shared<UpperWeightGreedyDestroy>(items, solution.get_valid_groups().size(), 50, 1, solution, destroy_ratio));
    
    init_destroy_random();

    for (auto&& d : destructions) {
        if (typeid(*d) == typeid(RandomDestroy) || typeid(*d) == typeid(MinimumDestroy)) {
            item_destroy.emplace_back(d);
        }
        else {
            group_destroy.emplace_back(d);
        }
    }
    
    dummy_items.reserve(Group::N);
    //�_�~�[�A�C�e���̐ݒ�. �A�C�e��id���A�C�e�����ȏ�ɐݒ�
    for (size_t i = 0; i < Group::N; ++i) {
        Item item;
        item.id = Item::N + i;
        dummy_items.push_back(item);
    }
}

/*�e���_�ԂɗL���ӂ�K�v�ɉ����Đݒ�*/
void NeighborhoodGraph::set_edge(Solution& solution) {
    //s����t�ւ̗L���ӂ�ݒ�
    #pragma omp for
    for (const auto& t : vertices) {
        for (const auto& s : vertices) {
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
                        graph[t.id].push_back(Edge(s.id, -weight));
                        //edge_ptr.push_back(&graph[s.id][graph[s.id].size() - 1]);
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
                        graph[t.id].push_back(Edge(s.id, -weight));
                        //edge_ptr.push_back(&graph[s.id][graph[s.id].size() - 1]);
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
                        graph[t.id].push_back(Edge(s.id, -weight));
                        //edge_ptr.push_back(&graph[s.id][graph[s.id].size() - 1]);
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
std::unique_ptr<Solution> NeighborhoodGraph::operator()(const Solution& current_solution) {
    is_move = false;

    auto neighborhood_solution = std::make_unique<Solution>(current_solution);
    size_t search_group_size;
    if (neighborhood_solution->get_eval_flags().test(Solution::EvalIdx::GROUP_COST)) {
        search_group_size = neighborhood_solution->get_valid_groups().size();
        if (search_group_size != Group::N) search_group_size += 1;
    }
    else {
        search_group_size = Group::N;
    }

    //�T���Ώۂ̃O���[�v����1�̂Ƃ�, �T�����Ȃ�
    if (search_group_size < 2) return std::move(neighborhood_solution);

    //�ΏۃA�C�e����vertices�ɐݒ�
    auto target_items = select_destroy()(*neighborhood_solution);
    for (size_t i = 0, size = target_items.size(); i < size; ++i) {
        vertices.push_back(Vertex(i, *target_items[i]));
    }
    MyRandom::shuffle(dummy_items);
    for (size_t i = 0, size = dummy_items.size() * destroy_ratio; i < size; ++i) {
        vertices.push_back(Vertex(i + target_items.size(), dummy_items[i]));
    }

    //�O���t�̏�����
    graph.assign(vertices.size(), vector<Edge>());

    vector<vector<vector<double>>> dp(vertices.size(), vector<vector<double>>(vertices.size(), vector<double>(search_group_size - 1, DBL_MAX)));
    using TablePos = std::tuple<int, int, int>;
    vector<vector<vector<TablePos>>> prev(vertices.size(), vector<vector<TablePos>>(vertices.size(), vector<TablePos>(search_group_size - 1, {-1, -1, -1})));
    vector<std::pair<double, TablePos>> start_pos;

    //�T���̌������̂��߂̃����_�֐�
    auto lambda = [](const double& a) { return a < 0 ? a : DBL_MAX; };
    
    int id = -1;

    #pragma omp parallel
    {
        set_edge(*neighborhood_solution);

        //DP�ŕ��H��T��. ���H�̒����̓O���[�v���ȉ��̂��̂��T���̑Ώ�
        #pragma omp for schedule(dynamic)
        for (const auto& v : vertices) {
            for (const auto& e : graph[v.id]) {
                dp[e.source][v.id][0] = lambda(e.weight);
            }
        }
        for (size_t l = 1, size = search_group_size - 1; l < size; ++l) {
            #pragma omp for schedule(dynamic)
            for (const auto& v1 : vertices) {
                for (const auto& v2 : vertices) {
                    for (const auto& e : graph[v2.id]) {
                        if (dp[v1.id][e.source][l - 1] != DBL_MAX && lambda(dp[v1.id][e.source][l - 1] + e.weight) < dp[v1.id][v2.id][l]) {
                            dp[v1.id][v2.id][l] = lambda(dp[v1.id][e.source][l - 1] + e.weight);
                            prev[v1.id][v2.id][l] = {v1.id, e.source, l - 1};
                        }
                    }
                }
            }
        }

        //DP�e�[�u�����畉�H���ɏd�݂̍��v�Ǝn�_�𒊏o
        #pragma omp for
        for (const auto& v : vertices) {
            for (const auto& e : graph[v.id]) {
                for (size_t l = 0, size = search_group_size - 1; l < size; ++l) {
                    if (dp[v.id][e.source][l] + e.weight < 0) {
                        #pragma omp critical
                        start_pos.push_back(std::make_pair(dp[v.id][e.source][l] + e.weight, std::make_tuple(v.id, e.source, l)));
                    }
                }
            }
        }

        #pragma omp single
        {
            std::sort(start_pos.begin(), start_pos.end(), [](const auto& a, const auto& b) {
                return a.first < b.first;
            });
        }

        //���H���\�z��, �����A�C�e����2��ȏ㌻��Ă��Ȃ�������
        #pragma omp for schedule(static,1)
        for (const auto& sp : start_pos) {
            bool break_flag;
            #pragma omp critical
            break_flag = is_move;
            if (break_flag) continue;

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
            bool is_duplicated = false;         //�����A�C�e����2��ȏ㌻��邩��\��bool�^
            unsigned int flag = 0;              //�A�C�e���̏o�����Ǘ�����r�b�g�t���O

            //�����A�C�e����2��ȏ㌻��Ă��Ȃ�������
            for (auto ritr = cycle.rbegin(), rend = --cycle.rend(); ritr != rend; ++ ritr) {
                const auto& item = vertices[*ritr].item;
                int now_group_id;
                if (item.id < Item::N) {
                    now_group_id = neighborhood_solution->get_group_id(item);
                    const auto& next_item = vertices[*std::next(ritr)].item;
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

                if (!(flag & (1<<item.id))) {                       //item.id�Ƀt���O�������Ă��Ȃ��ꍇ
                    //�t���O�𗧂Ă�
                    flag |= (1<<item.id);
                }
                else {                                              //item.id�Ƀt���O�������Ă���ꍇ
                    is_duplicated = true;
                    break;
                }
            }
            if (is_duplicated) continue;

            #pragma omp critical
            {
                if (!is_move) {
                    is_move = neighborhood_solution->move_check(move_items);
                }
            }
        }

        /*#pragma omp for schedule(static,1)
        for (size_t i = 0; i < start_pos.size(); ++i) {
            bool break_flag;
            #pragma omp critical
            break_flag = is_move;
            if (break_flag) continue;

            //���H�\�z
            vector<int> cycle;
            auto [v1, v2, l] = start_pos[i].second;
            cycle.reserve(l + 2);
            cycle.push_back(v1);
            auto pos = start_pos[i].second;
            while (std::get<0>(pos) != -1) {
                auto [now_v1, now_v2, now_l] = pos;
                cycle.push_back(now_v2);
                pos = prev[now_v1][now_v2][now_l];
            }
            cycle.push_back(v1);
            
            vector<MoveItem> move_items;
            move_items.reserve(l + 1);
            bool is_duplicated = false;         //�����A�C�e����2��ȏ㌻��邩��\��bool�^
            unsigned int flag = 0;              //�A�C�e���̏o�����Ǘ�����r�b�g�t���O

            //�����A�C�e����2��ȏ㌻��Ă��Ȃ�������
            for (auto ritr = cycle.rbegin(), rend = --cycle.rend(); ritr != rend; ++ ritr) {
                const auto& item = vertices[*ritr].item;
                int now_group_id;
                if (item.id < Item::N) {
                    now_group_id = neighborhood_solution->get_group_id(item);
                    const auto& next_item = vertices[*std::next(ritr)].item;
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

                if (!(flag & (1<<item.id))) {                       //item.id�Ƀt���O�������Ă��Ȃ��ꍇ
                    //�t���O�𗧂Ă�
                    flag |= (1<<item.id);
                }
                else {                                              //item.id�Ƀt���O�������Ă���ꍇ
                    is_duplicated = true;
                    break;
                }
            }
            if (is_duplicated) continue;

            #pragma omp critical
            {
                if (!is_move) {
                    is_move = (neighborhood_solution->calc_diff_eval(neighborhood_solution->evaluation_diff(move_items)) > 0);
                    id = i;
                }
            }
        }*/
    }

    //���H���\�z��, �����A�C�e����2��ȏ㌻��Ă��Ȃ�������
    /*for (size_t i = 0; i < start_pos.size(); ++i) {
        //���H�\�z
        vector<int> cycle;
        auto [v1, v2, l] = start_pos[i].second;
        cycle.reserve(l + 2);
        cycle.push_back(v1);
        auto pos = start_pos[i].second;
        while (std::get<0>(pos) != -1) {
            auto [now_v1, now_v2, now_l] = pos;
            cycle.push_back(now_v2);
            pos = prev[now_v1][now_v2][now_l];
        }
        cycle.push_back(v1);

        vector<MoveItem> move_items;
        move_items.reserve(l + 1);
        bool is_duplicated = false;                     //�����A�C�e����2��ȏ㌻��邩��\��bool�^
        unsigned int flag = 0;                          //�A�C�e���̏o�����Ǘ�����r�b�g�t���O

        //�����A�C�e����2��ȏ㌻��Ă��Ȃ�������
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

            if (!(flag & (1<<item.id))) {               //item.id�Ƀt���O�������Ă��Ȃ��ꍇ
                flag |= (1<<item.id);
            }
            else {                                      //item.id�Ƀt���O�������Ă���ꍇ
                is_duplicated = true;
                break;
            }
        }
        if (is_duplicated) continue;

        //���H�̈ړ������P���ɂȂ��Ă���ꍇ, �ړ����ă��[�v�𔲂���
        if ((is_move = neighborhood_solution->move_check(move_items))) {
            std::cerr << "id, i:" << id << "," << i << std::endl;
            break;
        }
    }*/
    //std::cout << std::endl;
    vertices.clear();
    /*auto p = destroy_random->get_probabilities();
    for (auto&& pp : p) {
        std::cout << pp * 100 << " ";
    }
    std::cout << std::endl;*/
    return std::move(neighborhood_solution);
}