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

/*グループ分けを管理する解*/
class Solution {
private:
    std::vector<Group> groups;                                                                  //グループの集合
    std::list<std::unique_ptr<const Group>> valid_groups;                                     //現在使われているグループの参照集合
    std::vector<int> item_group_ids;                                                            //それぞれのアイテムが所属するグループid
    double relation;                                                                            //このグループ分けの関係値
    double penalty;                                                                             //このグループ分けのペナルティ
    double ave_balance;                                                                         //各グループのvalueの平均値のばらつき
    double sum_balance;                                                                         //各グループのvalueの合計のばらつき
    std::vector<std::vector<std::optional<std::vector<double>>>> each_group_item_relation;      //それぞれのグループに対するitem_relation each_group_item_relation[アイテム][グループ].value()[type]
    std::vector<std::vector<std::optional<int>>> each_group_item_penalty;                       //それぞれのグループに対するitem_penalty each_group_item_penalty[アイテム][グループ]
    std::vector<double> aves;                                                                   //valueのアイテム単位での平均
    std::vector<double> sum_values;                                                             //valueの合計
    Input::Opt opt;
    std::vector<double> item_relation_params;
    std::vector<double> group_relation_params;
    std::vector<double> value_ave_params;
    std::vector<double> value_sum_params;
    int penalty_param;
    double group_num_param;
    double constant;
    std::bitset<8> eval_flags;

    void move_processing(const std::vector<MoveItem>& move_items, const std::tuple<double, double, double, double>& diff);  //移動処理
    void set_eval_value(int relation, int penalty, double ave_balance, double sum_balance);                                 //評価値の元となるrelation, penalty, ave_balance, sum_balamceの設定

public:
    enum EvalIdx {
        WEIGHT_PENA, ITEM_PENA, GROUP_PENA, ITEM_R, GROUP_R, VALUE_AVE, VALUE_SUM, GROUP_NUM
    };
    Solution(const Input& input);                                                                                         //コンストラクタ
    double get_eval_value() const;                                                                                              //評価値を取得
    double calc_diff_eval(const std::tuple<double, double, double, double>& diff) const;
    const std::vector<double>& get_ave() const;                                                                                 //valueのアイテム単位での平均を取得
    const std::vector<double>& get_sum_values() const;                                                                          //valueの合計を取得
    const std::vector<double>& get_each_group_item_relation(const Item& item, int group_id);                                    //each_group_item_relationの値を取得, なければ計算して取得
    int get_each_group_item_penalty(const Item& item, int group_id);                                                            //each_group_item_penaltyの値を取得, なければ計算して取得
    int get_group_id(const Item& item) const;                                                                                   //アイテムの所属するグループidを取得
    auto get_groups_range() const -> const std::pair<std::vector<Group>::const_iterator, std::vector<Group>::const_iterator>;   //ダミーグループを除く(Group::N)グループを取得
    auto get_valid_groups() const -> const std::list<std::unique_ptr<const Group>>&;                                            //現在使われているグループを取得
    const std::vector<Group>& get_groups() const;                                                                               //ダミーグループを含むすべてのグループを取得
    const Group& get_dummy_group() const;                                                                                       //ダミーグループを取得
    double evaluation_all(const std::vector<Item>& items);                                                                      //現在の解(グループ分け)を評価
    auto evaluation_diff(const std::vector<MoveItem>& move_items) -> std::tuple<double, double, double, double>;                //評価値の変化量を計算
    auto evaluation_shift(const Item& item, int group_id) -> std::tuple<double, double, double, double>;                        //shift移動時の評価値の変化量を計算
    auto evaluation_swap(const Item& item1, const Item& item2) -> std::tuple<double, double, double, double>;                   //swap移動時の評価値の変化量を計算
    bool shift_check(const Item& item, int group_id);                                                                           //shift移動するかどうかを調査し, 必要に応じて移動する
    bool swap_check(const Item& item1, const Item& item2);                                                                      //swap移動するかどうかを調査し, 必要に応じて移動する
    bool move_check(const std::vector<MoveItem>& move_items);                                                                   //move_itemsに基づいて移動するかどうかを調査し, 必要に応じて移動する
    void move(const std::vector<MoveItem>& move_items);                                                                         //move_itemsに基づいて移動する
    double get_relation() const;                                                                                                //関係値を取得
    double get_penalty() const;                                                                                                 //ペナルティを取得
    double get_ave_balance() const;                                                                                             //各グループのvalueの平均値のばらつきを取得
    double get_sum_balance() const;                                                                                             //各グループのvalueの合計のばらつきを取得                                                                                    //sum_balanceのパラメータを取得
    const std::vector<double>& get_group_relation_params() const;
    const std::bitset<8>& get_eval_flags() const;

    friend std::ostream& operator<<(std::ostream&, const Solution&);                                                            //解の出力用
};

/*評価値の元となるrelation, penalty, ave_balance, sum_balamceの設定*/
inline void Solution::set_eval_value(int relation, int penalty, double ave_balance, double sum_balance) {
    this->relation = relation;
    this->penalty = penalty;
    this->ave_balance = ave_balance;
    this->sum_balance = sum_balance;
}

/*評価値を取得*/
inline double Solution::get_eval_value() const {
    return relation - penalty * penalty_param + ave_balance + sum_balance + constant;
}

inline double Solution::calc_diff_eval(const std::tuple<double, double, double, double>& diff) const {
    return std::get<0>(diff) - std::get<1>(diff) * penalty_param + std::get<2>(diff) + std::get<3>(diff);
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
inline auto Solution::get_valid_groups() const -> const std::list<std::unique_ptr<const Group>>& {
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
    return relation;
}

/*ペナルティを取得*/
inline double Solution::get_penalty() const {
    return penalty;
}

/*各グループのvalueの平均値のばらつきを取得*/
inline double Solution::get_ave_balance() const {
    return ave_balance;
}

/*各グループのvalueの合計のばらつきを取得*/
inline double Solution::get_sum_balance() const {
    return sum_balance;
}


inline const std::vector<double>& Solution::get_group_relation_params() const {
    return group_relation_params;
}

inline const std::bitset<8>& Solution::get_eval_flags() const {
    return eval_flags;
}

#endif