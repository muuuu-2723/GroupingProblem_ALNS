#ifndef GROUP_H_
#define GROUP_H_

#include <list>
#include <vector>
#include <iostream>
#include <cfloat>
#include <Item.hpp>

/*アイテムを分けるグループ*/
class Group {
private:
    int id;                                             //グループのid
    std::list<int> member_id;                           //グループに所属するアイテムのidリスト
    int member_num;                                     //グループに所属するアイテムの数
    std::vector<double> sum_weight;                     //アイテムのweightの合計
    std::vector<double> sum_values;                     //アイテムのvalueの合計
    std::vector<double> upper_weight;                   //weightの合計の上限
    std::vector<double> lower_weight;                   //weightの合計の下限

public:
    static int N;                                                                                                       //グループ数
    static std::vector<double> weight_aves;
    Group(int group_id, const std::vector<double>& upper, const std::vector<double>& lower);                            //コンストラクタ
    Group(int group_id);
    Group(Item& leader, std::vector<Item>& member, int group_id);                                                       //メンバー指定のコンストラクタ
    void erase_member(const Item& item);                                                                                //所属するアイテムの削除
    void add_member(const Item& item);                                                                                  //新たにアイテムを追加する
    std::vector<double> item_relation(const Item& item, const std::vector<double>& params) const;                       //あるアイテムとこのグループのitem_relation
    std::vector<double> sum_item_relation(const std::vector<Item>& items, const std::vector<double>& params) const;     //このグループのitem_relationの合計
    std::vector<double> sum_group_relation(const std::vector<Item>& items, const std::vector<double>& params) const;    //このグループのgroup_relationの合計
    double diff_weight_penalty(const std::vector<const Item*>& add, const std::vector<const Item*>& erase) const;       //weight_penaltyの変化量
    double calc_weight_penalty() const;                                                                                 //このグループのweight_penaltyを計算
    int calc_item_penalty(const Item& item) const;                                                                      //あるアイテムとこのグループのitem_penalty
    int calc_sum_item_penalty(const std::vector<Item>& items) const;                                                    //このグループのitem_penaltyを計算
    int calc_group_penalty(const std::vector<Item>& items) const;                                                       //このグループのgroup_penaltyを計算
    int get_id() const;                                                                                                 //グループのidを取得
    int get_member_num() const;                                                                                         //グループに所属するアイテムの数を取得
    const std::vector<double>& get_sum_weight() const;                                                                  //アイテムのweightの合計を取得
    const std::vector<double>& get_sum_values() const;                                                                  //アイテムのvalueの合計を取得
    double value_average(size_t type) const;                                                                            //typeのvalueの平均を取得
    const std::vector<double>& get_upper() const;                                                                       //weightの上限を取得
    const std::vector<double>& get_lower() const;                                                                       //weightの下限を取得
    const std::list<int>& get_member_list() const;                                                                      //グループに所属するアイテムのidリストを取得

    friend std::ostream& operator<<(std::ostream&, const Group&);                                                       //グループの出力用
};

/*グループのid(番号)を取得*/
inline int Group::get_id() const {
    return id;
}

/*グループに所属するアイテムの数を取得*/
inline int Group::get_member_num() const {
    return member_num;
}

/*アイテムのweightの合計を取得*/
inline const std::vector<double>& Group::get_sum_weight() const {
    return sum_weight;
}

/*アイテムのvalueの合計を取得*/
inline const std::vector<double>& Group::get_sum_values() const {
    return sum_values;
}

/*typeのvalueの平均を取得*/
inline double Group::value_average(size_t type) const {
    return (double) sum_values[type] / member_num;
}

/*weightの上限を取得*/
inline const std::vector<double>& Group::get_upper() const {
    return upper_weight;
}

/*weightの下限を取得*/
inline const std::vector<double>& Group::get_lower() const {
    return lower_weight;
}

/*グループに所属するアイテムのidリストを取得*/
inline const std::list<int>& Group::get_member_list() const {
    return member_id;
}

#endif