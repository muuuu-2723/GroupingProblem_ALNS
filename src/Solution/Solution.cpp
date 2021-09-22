#include "Solution.hpp"
#include <Person.hpp>
#include <Group.hpp>
#include <MyRandom.hpp>
#include <Search.hpp>
#include <Destroy.hpp>

#include <vector>
#include <cmath>
#include <optional>
#include <iostream>
#include <numeric>
#include <cfloat>
#include <climits>
#include <tuple>
#include <iterator>
#include <memory>
#include <cassert>

using std::vector;

Solution::Solution(vector<Person>& persons) {
    Group::set_upper_and_lower(persons);
    penalty_parameter = 10000;
    relation_parameter = 40;
    score_parameter = 80;
    balance_parameter = 1;
    groups.clear();
    groups.reserve(Group::N + 1);
    for (int i = 0; i < Group::N + 1; i++) {
        groups.push_back(Group(i));
    }
    group_relation.assign(Person::N, vector<std::optional<int>>(Group::N + 1, std::nullopt));
    group_score_distance.assign(Person::N, vector<std::optional<int>>(Group::N + 1, std::nullopt));
    person_group_ids.resize(Person::N);
    ave = 0;

    //班長は指定の班, それ以外はランダムな班に追加
    RandomInt<> rand_group(0, Group::N - 1);
    int cnt = 0;
    for (auto&& person : persons) {
        if (person.is_leader) {
            groups[cnt].add_member(person);
            person_group_ids[person.id] = cnt++;
        }
        else {
            groups[Group::N].add_member(person);
            person_group_ids[person.id] = Group::N;
        }
        ave += person.score;
    }

    ave /= persons.size();

    //relation_greedy(persons);
    

    //penalty_greedy(persons);
    //score_greedy(persons);
    RelationGreedy rg(persons, 1);
    //PenaltyGreedy rg(persons, 1);
    std::shared_ptr<Destroy> des = std::make_shared<Destroy>(persons, 1);
    *this = std::move(rg(*this, des));
}

int Solution::get_group_relation(const Person& p, int group_id) {
    if (group_relation[p.id][group_id]) {
        return group_relation[p.id][group_id].value();
    }
    if (group_id >= Group::N) {
        group_relation[p.id][group_id] = 0;
        return group_relation[p.id][group_id].value();
    }
    group_relation[p.id][group_id] = groups[group_id].group_relation(p);
    return group_relation[p.id][group_id].value();
}

int Solution::get_group_score_distance(const Person& p, int group_id) {
    if (group_score_distance[p.id][group_id]) {
        return group_score_distance[p.id][group_id].value();
    }
    if (group_id >= Group::N) {
        group_score_distance[p.id][group_id] = 0;
        return group_score_distance[p.id][group_id].value();
    }
    group_score_distance[p.id][group_id] = groups[group_id].group_score_distance(p);
    return group_score_distance[p.id][group_id].value();
}

/*すべての班を評価*/
double Solution::evaluation_all(const vector<Person>& persons) {
    penalty = 0;
    relation = 0;
    balance = 0;
    deviation = 0;
    auto [group_begin, group_end] = get_groups_range();
    for (auto citr = group_begin; citr != group_end; ++citr) {
        penalty += citr->calc_penalty();

        balance += citr->group_scode_distance_all(persons);

        //点数の平均偏差用
        deviation += std::abs(ave - citr->score_average());

        //関係値と回数の和
        relation += citr->group_relation_all(persons);
    }
    deviation /= Group::N;

    return get_eval_value();
}

auto Solution::evaluation_diff(const vector<MovePerson>& move_persons) -> std::tuple<int, int, double, int> {
    vector<vector<const Person*>> in(Group::N + 1);
    vector<vector<const Person*>> out(Group::N + 1);
    for (const auto& mp : move_persons) {
        out[mp.source].push_back(&mp.p);
        in[mp.destination].push_back(&mp.p);
    }

    int diff_penalty = 0;
    for (int i = 0; i < Group::N; ++i) {
        diff_penalty += groups[i].diff_penalty(in[i], out[i]);
    }

    int decreace = 0, increace = 0, sd = 0;
    for (const auto& mp : move_persons) {
        decreace += get_group_relation(mp.p, mp.source);
        increace += get_group_relation(mp.p, mp.destination);
        sd -= get_group_score_distance(mp.p, mp.source);
        sd += get_group_score_distance(mp.p, mp.destination);
        for (const auto& ptr_p : out[mp.destination]) {
            decreace += mp.p.relations[ptr_p->id] - mp.p.times[ptr_p->id];
            sd -= mp.p.score_distances[ptr_p->id];
        }
    }
    for (int i = 0; i < Group::N; ++i) {
        for (auto itr1 = out[i].begin(), end = out[i].end(); itr1 != end; ++itr1) {
            for (auto itr2 = std::next(itr1); itr2 != end; ++itr2) {
                increace += (*itr1)->relations[(*itr2)->id] - (*itr1)->times[(*itr2)->id];
                sd += (*itr1)->score_distances[(*itr2)->id];
            }
        }
        for (auto itr1 = in[i].begin(), end = in[i].end(); itr1 != end; ++itr1) {
            for (auto itr2 = std::next(itr1); itr2 != end; ++itr2) {
                increace += (*itr1)->relations[(*itr2)->id] - (*itr1)->times[(*itr2)->id];
                sd += (*itr1)->score_distances[(*itr2)->id];
            }
        }
    }

    double diff_deviation = 0;
    //double next_dispersion = std::pow(deviation, 2);
    for (int i = 0; i < Group::N; ++i) {
        if (in[i].size() == 0 && out[i].size() == 0) continue;

        diff_deviation -= std::abs(groups[i].score_average() - ave) / Group::N;
        //next_dispersion -= std::pow(groups[i].score_average() - ave, 2);
        int new_sum_score = groups[i].get_sum_score();
        for (const auto& ptr_p : in[i]) {
            new_sum_score += ptr_p->score;
        }
        for (const auto& ptr_p : out[i]) {
            new_sum_score -= ptr_p->score;
        }
        double new_group_ave = (double)new_sum_score / (groups[i].get_member_num() - out[i].size() + in[i].size());
        diff_deviation += std::abs(new_group_ave - ave) / Group::N;
        //next_dispersion += std::pow(new_group_ave - ave, 2);
    }

    //double next_deviation = std::sqrt(next_dispersion);

    //計算誤差の対策
    if (std::abs(diff_deviation) < 1e-10 && increace == decreace && diff_penalty == 0 && sd == 0) {
        return {0, 0, 0, 0};
    }
    //std::cerr << increace - decreace << " " << diff_penalty << " " << next_deviation - std::sqrt(dispersion) << std::endl;

    //return increace - decreace - diff_penalty * penalty_parameter - (next_deviation - std::sqrt(dispersion)) * score_parameter;
    return {increace - decreace, diff_penalty, diff_deviation, sd};
}

/*shift移動時の評価値の変化量*/
auto Solution::evaluation_shift(const Person& p, int group_id) -> std::tuple<int, int, double, int> {
    //関係値と回数の和の変化量を計算
    const Group& now_group = groups[person_group_ids[p.id]];
    int decreace, increace;
    decreace = get_group_relation(p, now_group.get_id());

    const Group& next_group = groups[group_id];
    increace = get_group_relation(p, next_group.get_id());

    int sd = get_group_score_distance(p, next_group.get_id()) - get_group_score_distance(p, now_group.get_id());

    //ペナルティの変化量を計算
    /*int diff_penalty = now_group.erase_penalty(upper_num[p.year - 1][p.gender][p.campus], 
                                          lower_num[p.year - 1][p.gender][p.campus], p);
    
    diff_penalty += next_group.add_penalty(upper_num[p.year - 1][p.gender][p.campus],
                                      lower_num[p.year - 1][p.gender][p.campus], p);*/
    int diff_penalty = now_group.diff_penalty({}, {&p});
    diff_penalty += next_group.diff_penalty({&p}, {});
    
    //点数の平均偏差の算出
    double diff_deviation = 0;
    diff_deviation -= std::abs(now_group.score_average() - ave) / Group::N;
    diff_deviation -= std::abs(next_group.score_average() - ave) / Group::N;
    //double next_dispersion = std::pow(deviation, 2);
    //next_dispersion -= std::pow(now_group.score_average() - ave, 2);
    //next_dispersion -= std::pow(next_group.score_average() - ave, 2);

    double group_ave = (double)(now_group.get_sum_score() - p.score) / (now_group.get_member_num() - 1);
    diff_deviation += std::abs(group_ave - ave) / Group::N;
    //next_dispersion += std::pow(group_ave - ave, 2);
    group_ave = (double)(next_group.get_sum_score() + p.score) / (next_group.get_member_num() + 1);
    diff_deviation += std::abs(group_ave - ave) / Group::N;
    //next_dispersion += std::pow(group_ave - ave, 2);

    //double next_deviation = std::sqrt(next_dispersion);

    //計算誤差の対策
    if (std::abs(diff_deviation) < 1e-10 && increace == decreace && diff_penalty == 0, sd == 0) {
        return {0, 0, 0, 0};
    }

    return {increace - decreace, diff_penalty, diff_deviation, sd};
}

/*swap移動時の評価値の変化量*/
auto Solution::evaluation_swap(const Person& p1, const Person& p2) -> std::tuple<int, int, double, int> {
    //関係値と回数の和の変化量を計算(shift移動時の計算結果を利用)
    const Group& g1 = groups[person_group_ids[p1.id]];
    const Group& g2 = groups[person_group_ids[p2.id]];
    int diff_relation = 0, sd = 0;
    diff_relation += get_group_relation(p1, g2.get_id());
    diff_relation += get_group_relation(p2, g1.get_id());
    diff_relation -= get_group_relation(p1, g1.get_id());
    diff_relation -= get_group_relation(p2, g2.get_id());
    sd += get_group_score_distance(p1, g2.get_id());
    sd += get_group_score_distance(p2, g1.get_id());
    sd -= get_group_score_distance(p1, g1.get_id());
    sd -= get_group_score_distance(p2, g2.get_id());

    diff_relation -= (p1.relations[p2.id] - p1.times[p2.id]) * 2;
    sd -= p1.score_distances[p2.id] * 2;

    //ペナルティの変化量を計算
    int diff_penalty = 0;
    if (p1.year != p2.year || p1.gender != p2.gender || p1.campus != p2.campus) {
        /*diff_penalty += groups[p1.group_id].erase_penalty(upper_num[p1.year - 1][p1.gender][p1.campus],
                                                     lower_num[p1.year - 1][p1.gender][p1.campus], p1);
        
        diff_penalty += groups[p1.group_id].add_penalty(upper_num[p2.year - 1][p2.gender][p2.campus],
                                                   lower_num[p2.year - 1][p2.gender][p2.campus], p2);

        diff_penalty += groups[p2.group_id].erase_penalty(upper_num[p2.year - 1][p2.gender][p2.campus],
                                                     lower_num[p2.year - 1][p2.gender][p2.campus], p2);
        
        diff_penalty += groups[p2.group_id].add_penalty(upper_num[p1.year - 1][p1.gender][p1.campus],
                                                   lower_num[p1.year - 1][p1.gender][p1.campus], p1);


        diff_penalty -= 4;*/
        diff_penalty += g1.diff_penalty({&p2}, {&p1});
        diff_penalty += g2.diff_penalty({&p1}, {&p2});
    }

    //点数の平均偏差を算出
    double diff_deviation = 0;
    //double next_dispersion = std::pow(deviation, 2);
    diff_deviation -= std::abs(g1.score_average() - ave) / Group::N;
    diff_deviation -= std::abs(g2.score_average() - ave) / Group::N;
    //next_dispersion -= std::pow(g1.score_average() - ave, 2);
    //next_dispersion -= std::pow(g2.score_average() - ave, 2);

    double group_ave = (double)(g1.get_sum_score() - p1.score + p2.score) / g1.get_member_num();
    diff_deviation += std::abs(group_ave - ave) / Group::N;
    //next_dispersion += std::pow(group_ave - ave, 2);
    group_ave = (double)(g2.get_sum_score() - p2.score + p1.score) / g2.get_member_num();
    diff_deviation += std::abs(group_ave - ave) / Group::N;
    //next_dispersion += std::pow(group_ave - ave, 2);

    //double next_deviation = std::sqrt(next_dispersion);

    //計算誤差の対策
    if (std::abs(diff_deviation) < 1e-10 && diff_relation == 0 && diff_penalty == 0, sd == 0) {
        return {0, 0, 0, 0};
    }

    return {diff_relation, diff_penalty, diff_deviation, sd};
}

void Solution::move_processing(const std::vector<MovePerson>& move_persons, const std::tuple<int, int, double, int>& diff) {
    auto [diff_relation, diff_penalty, diff_deviation, diff_balance] = diff;
    set_eval_value(relation + diff_relation, penalty + diff_penalty, deviation + diff_deviation, balance + diff_balance);
    for (const auto& mp : move_persons) {
        for (int i = 0; i < Person::N; ++i) {
            if (group_relation[i][mp.source] && mp.source < Group::N) {
                group_relation[i][mp.source].value() -= (mp.p.relations[i] - mp.p.times[i]);
            }
            if (group_relation[i][mp.destination] && mp.destination < Group::N) {
                group_relation[i][mp.destination].value() += (mp.p.relations[i] - mp.p.times[i]);
            }
            if (group_score_distance[i][mp.source] && mp.source < Group::N) {
                group_score_distance[i][mp.source].value() -= mp.p.score_distances[i];
            }
            if (group_score_distance[i][mp.destination] && mp.destination < Group::N) {
                group_score_distance[i][mp.destination].value() += mp.p.score_distances[i];
            }
         }
    }
    for (const auto& mp : move_persons) {
        //std::cerr << mp.p.id << " ";
        assert(person_group_ids[mp.p.id] == mp.source);
        groups[mp.source].erase_member(mp.p);
        groups[mp.destination].add_member(mp.p);
        person_group_ids[mp.p.id] = mp.destination;
    }
}

/*
 *shift移動するかどうかを調査し, 必要に応じて移動する
 *移動した場合はtrue, していない場合はfalseを返す
 */
bool Solution::shift_check(Person& p, int group_id) {
    auto diff = evaluation_shift(p, group_id);
    auto [diff_relation, diff_penalty, diff_deviation, diff_balance] = diff;
    double increace = diff_relation * relation_parameter - diff_penalty * penalty_parameter - diff_deviation * score_parameter + diff_balance * balance_parameter;
    if (increace > 0 && std::abs(increace) > 1e-10) {
        vector<MovePerson> move_persons = {MovePerson(p, person_group_ids[p.id], group_id)};
        move_processing(move_persons, diff);
        //値の更新
        /*set_eval_value(relation + diff_relation, penalty + diff_penalty, deviation + diff_deviation);
        for (int i = 0; i < Person::N; i++) {
            if (group_relation[i][p.group_id]) {
                group_relation[i][p.group_id].value() -= (p.relations[i] - p.times[i]);
            }
            if (group_relation[i][group_id]) {
                group_relation[i][group_id].value() += (p.relations[i] - p.times[i]);
            }
        }

        //移動
        groups[p.group_id].erase_member(p);
        groups[group_id].add_member(p);*/
        
        return true;
    }
    return false;
}

/*
 *swap移動するかどうかを調査し, 必要に応じて移動する
 *移動した場合はtrue, していない場合はfalseを返す
 */
bool Solution::swap_check(Person& p1, Person& p2) {
    auto diff = evaluation_swap(p1, p2);
    auto [diff_relation, diff_penalty, diff_deviation, diff_balance] = diff;
    double increace = diff_relation * relation_parameter - diff_penalty * penalty_parameter - diff_deviation * score_parameter + diff_balance * balance_parameter;
    if (increace > 0 && std::abs(increace) > 1e-10) {
        int g1_id = person_group_ids[p1.id], g2_id = person_group_ids[p2.id];
        vector<MovePerson> move_persons = {MovePerson(p1, g1_id, g2_id), MovePerson(p2, g2_id, g1_id)};
        move_processing(move_persons, diff);
        /*int group_id1 = p1.group_id;
        int group_id2 = p2.group_id;

        //値の更新
        set_eval_value(relation + diff_relation, penalty + diff_penalty, deviation + diff_deviation);
        for (int i = 0; i < Person::N; i++) {
            //リーダー避け(スマートな方法に改善したい)
            if (group_relation[i][group_id1]) {
                group_relation[i][group_id1].value() -= (p1.relations[i] - p1.times[i]);
                group_relation[i][group_id1].value() += (p2.relations[i] - p2.times[i]);
                group_relation[i][group_id2].value() -= (p2.relations[i] - p2.times[i]);
                group_relation[i][group_id2].value() += (p1.relations[i] - p1.times[i]);
            }
        }

        //移動
        groups[group_id1].erase_member(p1);
        groups[group_id2].erase_member(p2);
        groups[group_id1].add_member(p2);
        groups[group_id2].add_member(p1);*/
        return true;
    }
    return false;
}

bool Solution::move_check(const vector<MovePerson>& move_persons) {
    auto diff = evaluation_diff(move_persons);
    auto [diff_relation, diff_penalty, diff_deviation, diff_balance] = diff;
    double increace = diff_relation * relation_parameter - diff_penalty * penalty_parameter - diff_deviation * score_parameter + diff_balance * balance_parameter;

    if (increace > 0) {
        move_processing(move_persons, diff);
        return true;
    }
    return false;
}

void Solution::move(const vector<MovePerson>& move_persons) {
    move_processing(move_persons, evaluation_diff(move_persons));
}

std::ostream& operator<<(std::ostream& out, const Solution& s) {
    auto [group_begin, group_end] = s.get_groups_range();
    for (auto citr = group_begin; citr != group_end; ++citr) {
        out << *citr << std::endl;
    }
    out << "評価値:" << s.get_eval_value() << std::endl;
    return out;
}

/*void Solution::relation_greedy(vector<Person>& persons) {
    for (auto&& person : persons) {
        if (person.is_leader) {
            groups[person.group_id].add_member(person);
        }
    }
    vector<int> shuffle_ids(persons.size());
    std::iota(shuffle_ids.begin(), shuffle_ids.end(), 0);
    MyRandom::shuffle(shuffle_ids);
    for (const auto& id : shuffle_ids) {
        if (!persons[id].is_leader) {
            int max_group_id;
            double max_value = -DBL_MAX;
            auto [group_begin, group_end] = get_groups_range();
            for (auto citr = group_begin; citr != group_end; ++citr) {
                double value;
                if ((value = citr->group_relation(persons[id]) / (double)citr->get_member_num()) > max_value) {
                    max_value = value;
                    max_group_id = citr->get_id();
                }
            }
            groups[max_group_id].add_member(persons[id]);
        }
    }
}

void Solution::penalty_greedy(vector<Person>& persons) {
    for (auto&& person : persons) {
        if (person.is_leader) {
            groups[person.group_id].add_member(person);
        }
    }
    vector<int> shuffle_ids(persons.size());
    std::iota(shuffle_ids.begin(), shuffle_ids.end(), 0);
    MyRandom::shuffle(shuffle_ids);
    for (const auto& id : shuffle_ids) {
        Person& p = persons[id];
        if (!p.is_leader) {
            int min_group_id;
            int min_penalty = INT_MAX;
            auto [group_begin, group_end] = get_groups_range();
            for (auto citr = group_begin; citr != group_end; ++citr) {
                int diff_penalty = citr->diff_penalty({&p}, {});
                if (diff_penalty < min_penalty) {
                    min_group_id = citr->get_id();
                    min_penalty = diff_penalty;
                }
            }
            //std::cerr << min_penalty << std::endl;
            groups[min_group_id].add_member(p);
        }
    }
}

void Solution::score_greedy(vector<Person>& persons) {
    std::sort(persons.begin(), persons.end(), [](const Person& a, const Person& b) {
        return a.score > b.score;
    });
    vector<vector<int>> blocks(Person::N / Group::N + 1);
    for (int i = 0; i < Person::N; i++) {
        blocks[i / Group::N].push_back(persons[i].id);
    }
    std::sort(persons.begin(), persons.end(), [](const Person& a, const Person& b) {
        return a.id < b.id;
    });
    for (int i = 0; i < blocks.size(); i++) {
        if (i % 4 == 1 || i % 4 == 2) {
            std::reverse(blocks[i].begin(), blocks[i].end());
        }
        for (int j = 0; j < blocks[i].size(); j++) {
            if (persons[blocks[i][j]].is_leader && persons[blocks[i][j]].group_id != j) {
                std::swap(blocks[i][j], blocks[i][persons[blocks[i][j]].group_id]);
            }
        }
    }

    for (auto&& block : blocks) {
        for (int i = 0; i < block.size(); i++) {
            groups[i].add_member(persons[block[i]]);
        }
    }
}*/