#ifndef GROUP_H_
#define GROUP_H_

#include <list>
#include <vector>
#include <iostream>

struct Item;

class Group {
private:
    int id;                                                                             //班のid:0～G-1(5)
    std::list<int> member_id;                                                           //班員のid
    int member_num;                                                                     //班員数
    std::vector<int> num_ratio;                               //人数比:num_ratio[学年][男女][多摩理工]
    std::vector<int> sum_values;                                                                      //点数の合計
    static std::vector<int> upper_num;
    static std::vector<int> lower_num;

public:
    static int N;                                                                       //班数
    Group(int group_id);
    Group(Item& leader, std::vector<Item>& member, int group_id);
    void erase_member(const Item& item);
    void add_member(const Item& item);
    int group_score_distance(const Item& item) const;
    int group_scode_distance_all(const std::vector<Item>& items) const;
    int group_relation(const Item& item) const;                                          //ある人とこの班の人との関係値と回数の和
    int group_relation_all(const std::vector<Item>& items) const;                   //この班の関係値と回数の和
    int add_penalty(int category_upper, int category_lower, const Item& item) const;     //ある人を班員に加えた時のペナルティ変化量
    int erase_penalty(int category_upper, int category_lower, const Item& item) const;   //班員pを削除した時のペナルティ変化量
    int diff_penalty(const std::vector<const Item*>& add, const std::vector<const Item*>& erase) const;
    int calc_penalty() const;
    static void set_upper_and_lower(const std::vector<Item>& items);
    int get_id() const;
    int get_member_num() const;
    const std::vector<int>& get_num_ratio() const;
    const std::vector<int>& get_sum_values() const;
    std::vector<double> value_averages() const;
    static const std::vector<int>& get_upper();
    static const std::vector<int>& get_lower();
    const std::list<int>& get_member_list() const;

    friend std::ostream& operator<<(std::ostream&, const Group&);
};


/*班のid(番号)を取得*/
inline int Group::get_id() const {
    return id;
}

/*班員数を取得*/
inline int Group::get_member_num() const {
    return member_num;
}

/*人数比を取得*/
inline const std::vector<int>& Group::get_num_ratio() const {
    return num_ratio;
}

/*合計点を取得*/
inline const std::vector<int>& Group::get_sum_values() const {
    return sum_values;
}

/*平均点を取得*/
inline std::vector<double> Group::value_averages() const {
    std::vector<double> aves(sum_values.size());
    for (size_t i = 0; i < aves.size(); ++i) {
        aves[i] = (double) sum_values[i] / member_num;
    }
    return std::move(aves);
}

inline const std::vector<int>& Group::get_upper() {
    return upper_num;
}

inline const std::vector<int>& Group::get_lower() {
    return lower_num;
}

inline const std::list<int>& Group::get_member_list() const {
    return member_id;
}

#endif