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
    std::vector<double> sum_weight;                               //人数比:num_ratio[学年][男女][多摩理工]
    std::vector<double> sum_values;                                                                      //点数の合計
    static std::vector<double> upper_weight;
    static std::vector<double> lower_weight;

public:
    static int N;                                                                       //班数
    Group(int group_id);
    Group(Item& leader, std::vector<Item>& member, int group_id);
    void erase_member(const Item& item);
    void add_member(const Item& item);
    std::vector<double> item_relation(const Item& item) const;
    std::vector<double> sum_relation(const std::vector<Item>& items) const;
    double diff_penalty(const std::vector<const Item*>& add, const std::vector<const Item*>& erase) const;
    double calc_penalty() const;
    static void set_upper_and_lower(const std::vector<double>& upper, const std::vector<double>& lower);
    int get_id() const;
    int get_member_num() const;
    const std::vector<double>& get_sum_weight() const;
    const std::vector<double>& get_sum_values() const;
    std::vector<double> value_averages() const;
    static const std::vector<double>& get_upper();
    static const std::vector<double>& get_lower();
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
inline const std::vector<double>& Group::get_sum_weight() const {
    return sum_weight;
}

/*合計点を取得*/
inline const std::vector<double>& Group::get_sum_values() const {
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

inline const std::vector<double>& Group::get_upper() {
    return upper_weight;
}

inline const std::vector<double>& Group::get_lower() {
    return lower_weight;
}

inline const std::list<int>& Group::get_member_list() const {
    return member_id;
}

#endif