#ifndef ITEM_H_
#define ITEM_H_

#include <vector>

struct Item {
    static int N;                                       //アイテム数
    static std::vector<int> num_category;

    int id;                                             //0～N-1
    int predefined_group;                               //あらかじめ決められた固定グループ(-1 : 該当なし)
    std::vector<int> category;                          //各アイテムが所属するカテゴリ
    std::vector<double> values;                         //各アイテムがもつ値
    std::vector<std::vector<double>> item_relations;    //それぞれのアイテム間の関係値
    std::vector<std::vector<double>> group_relations;   //アイテムとグループの間の関係値

    int get_category_num() const;
};

inline int Item::get_category_num() const {
    int num = 0;
    int tmp = 1;
    for (size_t i = 0; i < num_category.size(); ++i) {
        num += category[i] * tmp;
        tmp *= num_category[i];
    }
    return num;
}

#endif