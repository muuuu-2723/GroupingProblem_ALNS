#ifndef SOLUTION_H_
#define SOLUTION_H_

#include <Group.hpp>
#include <Item.hpp>
#include <vector>
#include <optional>
#include <iostream>
#include <tuple>
#include <cmath>

struct MoveItem {
    Item& item;
    int source, destination;
    MoveItem(Item& item, int source, int destination) : item(item), source(source), destination(destination) {}
};

enum class Moving {
    NoMove,
    WorseMove,
    BetterMove
};

class Solution {
private:
    std::vector<Group> groups;                              //�Ǖ���
    std::vector<int> item_group_ids;
    int relation;
    int penalty;
    double deviation;
    int balance;
    std::vector<std::vector<std::optional<int>>> group_relation;           //����l�Ɣǈ��̊֌W�l�Ɖ񐔂̘a
    std::vector<std::vector<std::optional<int>>> group_score_distance;
    double ave;
    int relation_parameter;
    int penalty_parameter;
    int score_parameter;
    int balance_parameter;

    void move_processing(const std::vector<MoveItem>& move_items, const std::tuple<int, int, double, int>& diff);
    void set_eval_value(int relation, int penalty, double deviation, int balance);

public:
    Solution(std::vector<Item>& items);
    double get_eval_value() const;
    double get_ave() const;
    int get_group_relation(const Item& item, int group_id);
    int get_group_score_distance(const Item& item, int group_id);
    int get_group_id(const Item& item) const;
    auto get_groups_range() const -> const std::pair<std::vector<Group>::const_iterator, std::vector<Group>::const_iterator>;
    const std::vector<Group>& get_groups() const;
    const Group& get_dummy_group() const;
    double evaluation_all(const std::vector<Item>& items);
    auto evaluation_diff(const std::vector<MoveItem>& move_items) -> std::tuple<int, int, double, int>;
    auto evaluation_shift(const Item& item, int group_id) -> std::tuple<int, int, double, int>;
    auto evaluation_swap(const Item& item1, const Item& item2) -> std::tuple<int, int, double, int>;
    bool shift_check(Item& item, int group_id);
    bool swap_check(Item& item1, Item& item2);
    bool move_check(const std::vector<MoveItem>& move_items);
    void move(const std::vector<MoveItem>& move_items);
    /*void relation_greedy(std::vector<Item>& items);
    void penalty_greedy(std::vector<Item>& items);
    void score_greedy(std::vector<Item>& items);*/

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

inline int Solution::get_group_id(const Item& item) const {
    return item_group_ids[item.id];
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