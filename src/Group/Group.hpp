#ifndef GROUP_H_
#define GROUP_H_

#include <list>
#include <vector>
#include <iostream>

struct Item;

class Group {
private:
    int id;                                                                             //ÇÌid:0`G-1(5)
    std::list<int> member_id;                                                           //ÇõÌid
    int member_num;                                                                     //Çõ
    std::vector<double> sum_weight;                               //lä:num_ratio[wN][j][½H]
    std::vector<double> sum_values;                                                                      //_Ìv
    static std::vector<double> upper_weight;
    static std::vector<double> lower_weight;

public:
    static int N;                                                                       //Ç
    Group(int group_id);
    Group(Item& leader, std::vector<Item>& member, int group_id);
    void erase_member(const Item& item);
    void add_member(const Item& item);
    std::vector<double> item_relation(const Item& item) const;
    std::vector<double> sum_relation(const std::vector<Item>& items) const;
    double diff_weight_penalty(const std::vector<const Item*>& add, const std::vector<const Item*>& erase) const;
    double calc_weight_penalty() const;
    int calc_item_penalty(const Item& item) const;
    int calc_sum_item_penalty(const std::vector<Item>& items) const;
    int calc_group_penalty(const std::vector<Item>& items) const;
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


/*ÇÌid(Ô)ðæ¾*/
inline int Group::get_id() const {
    return id;
}

/*Çõðæ¾*/
inline int Group::get_member_num() const {
    return member_num;
}

/*läðæ¾*/
inline const std::vector<double>& Group::get_sum_weight() const {
    return sum_weight;
}

/*v_ðæ¾*/
inline const std::vector<double>& Group::get_sum_values() const {
    return sum_values;
}

/*½Ï_ðæ¾*/
inline std::vector<double> Group::value_averages() const {
    std::vector<double> aves(Item::v_size);
    for (size_t i = 0; i < Item::v_size; ++i) {
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