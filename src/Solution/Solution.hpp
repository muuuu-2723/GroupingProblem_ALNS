#ifndef SOLUTION_H_
#define SOLUTION_H_

#include <Group.hpp>
#include <Person.hpp>
#include <vector>
#include <optional>
#include <iostream>
#include <tuple>
#include <cmath>

struct MovePerson {
    Person& p;
    int source, destination;
    MovePerson(Person& p, int source, int destination) : p(p), source(source), destination(destination) {}
};

enum class Moving {
    NoMove,
    WorseMove,
    BetterMove
};

class Solution {
private:
    std::vector<Group> groups;                              //î«ï™ÇØ
    std::vector<int> person_group_ids;
    int relation;
    int penalty;
    double deviation;
    int balance;
    std::vector<std::vector<std::optional<int>>> group_relation;           //Ç†ÇÈêlÇ∆î«àıÇÃä÷åWílÇ∆âÒêîÇÃòa
    std::vector<std::vector<std::optional<int>>> group_score_distance;
    double ave;
    int relation_parameter;
    int penalty_parameter;
    int score_parameter;
    int balance_parameter;

    void move_processing(const std::vector<MovePerson>& move_persons, const std::tuple<int, int, double, int>& diff);
    void set_eval_value(int relation, int penalty, double deviation, int balance);

public:
    Solution(std::vector<Person>& persons);
    double get_eval_value() const;
    double get_ave() const;
    int get_group_relation(const Person& p, int group_id);
    int get_group_score_distance(const Person& p, int group_id);
    int get_group_id(const Person& p) const;
    auto get_groups_range() const -> const std::pair<std::vector<Group>::const_iterator, std::vector<Group>::const_iterator>;
    const std::vector<Group>& get_groups() const;
    const Group& get_dummy_group() const;
    double evaluation_all(const std::vector<Person>& persons);
    auto evaluation_diff(const std::vector<MovePerson>& move_persons) -> std::tuple<int, int, double, int>;
    auto evaluation_shift(const Person& p, int group_id) -> std::tuple<int, int, double, int>;
    auto evaluation_swap(const Person& p1, const Person& p2) -> std::tuple<int, int, double, int>;
    bool shift_check(Person& p, int group_id);
    bool swap_check(Person& p1, Person& p2);
    bool move_check(const std::vector<MovePerson>& move_persons);
    void move(const std::vector<MovePerson>& move_persons);
    /*void relation_greedy(std::vector<Person>& persons);
    void penalty_greedy(std::vector<Person>& persons);
    void score_greedy(std::vector<Person>& persons);*/

    friend std::ostream& operator<<(std::ostream&, const Solution&);
    int get_relation() const;
    int get_penalty() const;
    double get_deviation() const;
    int get_penalty_parameter() const;
    int get_balance_parameter() const;
    int get_relation_parameter() const;
    int get_score_parameter() const;
};

inline void Solution::set_eval_value(int relation, int penalty, double deviation, int balance) {
    this->relation = relation;
    this->penalty = penalty;
    this->deviation = deviation;
    this->balance = balance;
}

inline double Solution::get_eval_value() const {
    return relation * relation_parameter - penalty * penalty_parameter - deviation * score_parameter + balance * balance_parameter;
}

inline double Solution::get_ave() const {
    return ave;
}

inline int Solution::get_group_id(const Person& p) const {
    return person_group_ids[p.id];
}

inline auto Solution::get_groups_range() const -> const std::pair<std::vector<Group>::const_iterator, std::vector<Group>::const_iterator> {
    return {groups.cbegin(), groups.cbegin() + Group::N};
}

inline const std::vector<Group>& Solution::get_groups() const {
    return groups;
}

inline const Group& Solution::get_dummy_group() const {
    return groups[Group::N];
}

inline int Solution::get_relation() const {
    return relation;
}

inline int Solution::get_penalty() const {
    return penalty;
}

inline double Solution::get_deviation() const {
    return deviation;
}

inline int Solution::get_penalty_parameter() const {
    return penalty_parameter;
}

inline int Solution::get_balance_parameter() const {
    return balance_parameter;
}

inline int Solution::get_relation_parameter() const {
    return relation_parameter;
}

inline int Solution::get_score_parameter() const {
    return score_parameter;
}

#endif