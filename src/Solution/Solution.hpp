#ifndef SOLUTION_H_
#define SOLUTION_H_

#include <Group.hpp>
#include <Item.hpp>
#include <Input.hpp>
#include <vector>
#include <optional>
#include <iostream>
#include <tuple>
#include <cmath>
#include <memory>
#include <list>
#include <bitset>

/*アイテムの移動情報*/
struct MoveItem {
    const Item& item;       //移動するアイテム
    int source;             //移動元のグループid
    int destination;        //移動先のグループid

    /*コンストラクタ*/
    MoveItem(const Item& item, int source, int destination) : item(item), source(source), destination(destination) {}
};

struct EvalVals {
    double relation;
    double penalty;
    double ave_balance;
    double sum_balance;
    double group_cost;
    int group_num;

    EvalVals() : relation(0), penalty(0), ave_balance(0), sum_balance(0), group_cost(0), group_num(0) {}

    double get_value(double penalty_param, double constant) const {
        return relation - penalty * penalty_param + ave_balance / group_num + sum_balance / group_num + group_cost + constant;
    }

    EvalVals& operator+=(const EvalVals& ev) {
        this->relation += ev.relation;
        this->penalty += ev.penalty;
        this->ave_balance += ev.ave_balance;
        this->sum_balance += ev.sum_balance;
        this->group_cost += ev.group_cost;
        this->relation = std::abs(this->relation) < 1e-10 ? 0 : this->relation;
        this->penalty = std::abs(this->penalty) < 1e-10 ? 0 : this->penalty;
        this->ave_balance = std::abs(this->ave_balance) < 1e-10 ? 0 : this->ave_balance;
        this->sum_balance = std::abs(this->sum_balance) < 1e-10 ? 0 : this->sum_balance;
        this->group_num += ev.group_num;
        return *this;
    }
};

/*グループ分けを管理する解*/
class Solution {
private:
    std::vector<Group> groups;                                                                  //グループの集合
    std::list<const Group*> valid_groups;                                                       //現在使われているグループの参照集合
    std::vector<int> item_group_ids;                                                            //それぞれのアイテムが所属するグループid
    EvalVals eval;
    std::vector<std::vector<std::optional<double>>> each_group_item_relation;                   //それぞれのグループに対するitem_relation each_group_item_relation[アイテム][グループ].value()
    std::vector<std::vector<std::optional<int>>> each_group_item_penalty;                       //それぞれのグループに対するitem_penalty each_group_item_penalty[アイテム][グループ]
    std::vector<double> aves;                                                                   //valueのアイテム単位での平均
    std::vector<double> sum_values;                                                             //valueの合計
    Input::Opt opt;                                                                             //最小化か最大化か
    std::vector<double> value_ave_params;                                                       //各グループのvalueの平均値のばらつきのパラメータ
    std::vector<double> value_sum_params;                                                       //各グループのvalueの合計のばらつきのパラメータ
    int penalty_param;                                                                          //ペナルティのパラメータ
    std::vector<double> group_cost;                                                             //グループ数のパラメータ
    double constant;                                                                            //目的関数の定数
    std::bitset<8> eval_flags;                                                                  //各評価値を計算する必要があるかを管理するフラグ

    void move_processing(const std::vector<MoveItem>& move_items, const EvalVals& diff);     //移動処理
    std::vector<std::vector<int>> item_times;
    std::vector<std::vector<int>> group_times;
    std::vector<std::vector<bool>> same_group;

public:
    enum EvalIdx {
        WEIGHT_PENA, ITEM_PENA, GROUP_PENA, ITEM_R, GROUP_R, VALUE_AVE, VALUE_SUM, GROUP_COST
    };
    Solution(const Input& input);                                                                                                   //コンストラクタ
    Solution(const Solution& s);
    Solution(Solution&& s) = delete;
    Solution& operator=(const Solution& s);
    Solution& operator=(Solution&& s) = delete;
    double get_eval_value() const;                                                                                                  //評価値を取得
    double calc_diff_eval(const EvalVals& diff) const;                                    //変化量に対する評価値を計算
    const std::vector<double>& get_ave() const;                                                                                     //valueのアイテム単位での平均を取得
    const std::vector<double>& get_sum_values() const;                                                                              //valueの合計を取得
    double get_each_group_item_relation(const Item& item, int group_id);                                                            //each_group_item_relationの値を取得, なければ計算して取得
    int get_each_group_item_penalty(const Item& item, int group_id);                                                                //each_group_item_penaltyの値を取得, なければ計算して取得
    int get_group_id(const Item& item) const;                                                                                       //アイテムの所属するグループidを取得
    auto get_groups_range() const -> const std::pair<std::vector<Group>::const_iterator, std::vector<Group>::const_iterator>;       //ダミーグループを除く(Group::N)グループを取得
    auto get_valid_groups() const -> const std::list<const Group*>&;                                                                //現在使われているグループを取得
    const std::vector<Group>& get_groups() const;                                                                                   //ダミーグループを含むすべてのグループを取得
    const Group& get_dummy_group() const;                                                                                           //ダミーグループを取得
    double evaluation_all(const std::vector<Item>& items);                                                                          //現在の解(グループ分け)を評価
    EvalVals evaluation_diff(const std::vector<MoveItem>& move_items);            //評価値の変化量を計算
    EvalVals evaluation_shift(const Item& item, int group_id);                    //shift移動時の評価値の変化量を計算
    EvalVals evaluation_swap(const Item& item1, const Item& item2);               //swap移動時の評価値の変化量を計算
    bool shift_check(const Item& item, int group_id);                                                                               //shift移動するかどうかを調査し, 必要に応じて移動する
    bool swap_check(const Item& item1, const Item& item2);                                                                          //swap移動するかどうかを調査し, 必要に応じて移動する
    bool move_check(const std::vector<MoveItem>& move_items);                                                                       //move_itemsに基づいて移動するかどうかを調査し, 必要に応じて移動する
    void move(const std::vector<MoveItem>& move_items);                                                                             //move_itemsに基づいて移動する
    void shift_move(const Item& item, int group_id);
    void swap_move(const Item& item1, const Item& item2);
    double get_relation() const;                                                                                                    //関係値を取得
    double get_penalty() const;                                                                                                     //ペナルティを取得
    double get_ave_balance() const;                                                                                                 //各グループのvalueの平均値のばらつきを取得
    double get_sum_balance() const;                                                                                                 //各グループのvalueの合計のばらつきを取得
    double get_sum_group_cost() const;
    const std::vector<double>& get_group_relation_params() const;                                                                   //アイテムとグループ間の関係値のパラメータを取得
    const std::bitset<8>& get_eval_flags() const;                                                                                   //eval_flagsを取得
    const std::vector<std::vector<bool>>& get_same_group() const;

    friend std::ostream& operator<<(std::ostream&, const Solution&);                                                                //解の出力用
    auto get_item_times() const -> const std::vector<std::vector<int>>&;
    auto get_group_times() const -> const std::vector<std::vector<int>>&;
    void counter();
    int debug_same_group() const;
};

inline Solution& Solution::operator=(const Solution& s) {
    std::cout << "コピー代入演算子" << std::endl;
    groups = s.groups;
    item_group_ids = s.item_group_ids;
    eval = s.eval;
    each_group_item_relation = s.each_group_item_relation;
    each_group_item_penalty = s.each_group_item_penalty;
    aves = s.aves;
    sum_values = s.sum_values;
    opt = s.opt;
    value_ave_params = s.value_ave_params;
    value_sum_params = s.value_sum_params;
    penalty_param = s.penalty_param;
    group_cost = s.group_cost;
    constant = s.constant;
    eval_flags = s.eval_flags;
    item_times = s.item_times;
    group_times = s.group_times;
    same_group = s.same_group;

    valid_groups.clear();
    for (auto&& g_ptr : s.valid_groups) {
        valid_groups.push_back(&groups[g_ptr->get_id()]);
    }

    return *this;
}

EvalVals operator+(const EvalVals& ev1, const EvalVals& ev2);

int distance(const Solution& s1, const Solution& s2);

/*評価値を取得*/
inline double Solution::get_eval_value() const {
    return eval.get_value(penalty_param, constant);
}

/*変化量に対する評価値を計算*/
inline double Solution::calc_diff_eval(const EvalVals& diff) const {
    return (eval + diff).get_value(penalty_param, constant) - eval.get_value(penalty_param, constant);
}

/*valueのアイテム単位での平均を取得*/
inline const std::vector<double>& Solution::get_ave() const {
    return aves;
}

/*valueの合計を取得*/
inline const std::vector<double>& Solution::get_sum_values() const {
    return sum_values;
}

/*アイテムの所属するグループidを取得*/
inline int Solution::get_group_id(const Item& item) const {
    return item_group_ids[item.id];
}

/*ダミーグループを除く(Group::N)グループを取得*/
inline auto Solution::get_groups_range() const -> const std::pair<std::vector<Group>::const_iterator, std::vector<Group>::const_iterator> {
    return {groups.cbegin(), groups.cbegin() + Group::N};
}

/*現在使われているグループを取得*/
inline auto Solution::get_valid_groups() const -> const std::list<const Group*>& {
    return valid_groups;
}

/*ダミーグループを含むすべてのグループを取得*/
inline const std::vector<Group>& Solution::get_groups() const {
    return groups;
}

/*ダミーグループを取得*/
inline const Group& Solution::get_dummy_group() const {
    return groups[Group::N];
}

/*関係値を取得*/
inline double Solution::get_relation() const {
    return eval.relation;
}

/*ペナルティを取得*/
inline double Solution::get_penalty() const {
    return eval.penalty;
}

/*各グループのvalueの平均値のばらつきを取得*/
inline double Solution::get_ave_balance() const {
    return eval.ave_balance;
}

/*各グループのvalueの合計のばらつきを取得*/
inline double Solution::get_sum_balance() const {
    return eval.sum_balance;
}

inline double Solution::get_sum_group_cost() const {
    return eval.group_cost;
}

/*eval_flagsを取得*/
inline const std::bitset<8>& Solution::get_eval_flags() const {
    return eval_flags;
}

inline const std::vector<std::vector<bool>>& Solution::get_same_group() const {
    return same_group;
}

inline auto Solution::get_item_times() const -> const std::vector<std::vector<int>>& {
    return item_times;
}

inline auto Solution::get_group_times() const -> const std::vector<std::vector<int>>& {
    return group_times;
}

inline int Solution::debug_same_group() const {
    int result = 0;
    for (size_t i = 0, size = same_group.size(); i < size; ++i) {
        for (size_t j = i + 1; j < size; ++j) {
            if (same_group[i][j]) ++result;
        }
    }
    return result;
}

#endif