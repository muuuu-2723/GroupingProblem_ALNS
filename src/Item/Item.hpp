#ifndef ITEM_H_
#define ITEM_H_

#include <vector>

/*グループ分けするアイテム*/
struct Item {
    static size_t N;                                    //アイテム数
    static size_t w_size;                               //weightの種類数
    static size_t v_size;                               //valueの種類数
    static size_t item_r_size;                          //item_relationの種類数
    static size_t group_r_size;                         //group_relationの種類数

    size_t id;                                          //0～N-1
    int predefined_group;                               //あらかじめ決められた固定グループ(-1 : 該当なし)
    std::vector<double> weight;                         //各アイテムの重み(制約条件, ペナルティ用)
    std::vector<double> values;                         //各アイテムがもつ値(目的関数用)
    std::vector<std::vector<double>> item_relations;    //それぞれのアイテム間の関係値  item_relation[item_id][type]
    std::vector<std::vector<double>> group_relations;   //アイテムとグループの間の関係値  group_relation[group_id][type]
    std::vector<int> item_penalty;                      //それぞれのアイテム間のペナルティ値
    std::vector<int> group_penalty;                     //アイテムとグループの間のペナルティ値
};

#endif