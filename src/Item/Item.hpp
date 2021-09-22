#ifndef ITEM_H_
#define ITEM_H_

#include <vector>

struct Item {
    static int N;                                       //�A�C�e����
    static std::vector<int> num_category;

    int id;                                             //0�`N-1
    int predefined_group;                               //���炩���ߌ��߂�ꂽ�Œ�O���[�v(-1 : �Y���Ȃ�)
    std::vector<int> category;                          //�e�A�C�e������������J�e�S��
    std::vector<double> values;                         //�e�A�C�e�������l
    std::vector<std::vector<double>> item_relations;    //���ꂼ��̃A�C�e���Ԃ̊֌W�l
    std::vector<std::vector<double>> group_relations;   //�A�C�e���ƃO���[�v�̊Ԃ̊֌W�l

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