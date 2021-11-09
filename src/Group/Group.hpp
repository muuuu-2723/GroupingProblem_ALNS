#ifndef GROUP_H_
#define GROUP_H_

#include <list>
#include <vector>
#include <iostream>
#include <cfloat>
#include <Item.hpp>

/*�A�C�e���𕪂���O���[�v*/
class Group {
private:
    int id;                                             //�O���[�v��id
    std::list<int> member_id;                           //�O���[�v�ɏ�������A�C�e����id���X�g
    int member_num;                                     //�O���[�v�ɏ�������A�C�e���̐�
    std::vector<double> sum_weight;                     //�A�C�e����weight�̍��v
    std::vector<double> sum_values;                     //�A�C�e����value�̍��v
    std::vector<double> upper_weight;                   //weight�̍��v�̏��
    std::vector<double> lower_weight;                   //weight�̍��v�̉���

public:
    static int N;                                                                                                       //�O���[�v��
    static std::vector<double> weight_aves;
    Group(int group_id, const std::vector<double>& upper, const std::vector<double>& lower);                            //�R���X�g���N�^
    Group(int group_id);
    Group(Item& leader, std::vector<Item>& member, int group_id);                                                       //�����o�[�w��̃R���X�g���N�^
    void erase_member(const Item& item);                                                                                //��������A�C�e���̍폜
    void add_member(const Item& item);                                                                                  //�V���ɃA�C�e����ǉ�����
    std::vector<double> item_relation(const Item& item, const std::vector<double>& params) const;                       //����A�C�e���Ƃ��̃O���[�v��item_relation
    std::vector<double> sum_item_relation(const std::vector<Item>& items, const std::vector<double>& params) const;     //���̃O���[�v��item_relation�̍��v
    std::vector<double> sum_group_relation(const std::vector<Item>& items, const std::vector<double>& params) const;    //���̃O���[�v��group_relation�̍��v
    double diff_weight_penalty(const std::vector<const Item*>& add, const std::vector<const Item*>& erase) const;       //weight_penalty�̕ω���
    double calc_weight_penalty() const;                                                                                 //���̃O���[�v��weight_penalty���v�Z
    int calc_item_penalty(const Item& item) const;                                                                      //����A�C�e���Ƃ��̃O���[�v��item_penalty
    int calc_sum_item_penalty(const std::vector<Item>& items) const;                                                    //���̃O���[�v��item_penalty���v�Z
    int calc_group_penalty(const std::vector<Item>& items) const;                                                       //���̃O���[�v��group_penalty���v�Z
    int get_id() const;                                                                                                 //�O���[�v��id���擾
    int get_member_num() const;                                                                                         //�O���[�v�ɏ�������A�C�e���̐����擾
    const std::vector<double>& get_sum_weight() const;                                                                  //�A�C�e����weight�̍��v���擾
    const std::vector<double>& get_sum_values() const;                                                                  //�A�C�e����value�̍��v���擾
    double value_average(size_t type) const;                                                                            //type��value�̕��ς��擾
    const std::vector<double>& get_upper() const;                                                                       //weight�̏�����擾
    const std::vector<double>& get_lower() const;                                                                       //weight�̉������擾
    const std::list<int>& get_member_list() const;                                                                      //�O���[�v�ɏ�������A�C�e����id���X�g���擾

    friend std::ostream& operator<<(std::ostream&, const Group&);                                                       //�O���[�v�̏o�͗p
};

/*�O���[�v��id(�ԍ�)���擾*/
inline int Group::get_id() const {
    return id;
}

/*�O���[�v�ɏ�������A�C�e���̐����擾*/
inline int Group::get_member_num() const {
    return member_num;
}

/*�A�C�e����weight�̍��v���擾*/
inline const std::vector<double>& Group::get_sum_weight() const {
    return sum_weight;
}

/*�A�C�e����value�̍��v���擾*/
inline const std::vector<double>& Group::get_sum_values() const {
    return sum_values;
}

/*type��value�̕��ς��擾*/
inline double Group::value_average(size_t type) const {
    return (double) sum_values[type] / member_num;
}

/*weight�̏�����擾*/
inline const std::vector<double>& Group::get_upper() const {
    return upper_weight;
}

/*weight�̉������擾*/
inline const std::vector<double>& Group::get_lower() const {
    return lower_weight;
}

/*�O���[�v�ɏ�������A�C�e����id���X�g���擾*/
inline const std::list<int>& Group::get_member_list() const {
    return member_id;
}

#endif