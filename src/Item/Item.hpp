#ifndef ITEM_H_
#define ITEM_H_

#include <vector>

struct Item {
    static size_t N;                                    //�A�C�e����
    static size_t w_size;                               //weight�̎�ސ�
    static size_t v_size;                               //value�̎�ސ�
    static size_t item_r_size;                          //item_relation�̎�ސ�
    static size_t group_r_size;                         //group_relation�̎�ސ�

    size_t id;                                          //0�`N-1
    size_t predefined_group;                            //���炩���ߌ��߂�ꂽ�Œ�O���[�v(-1 : �Y���Ȃ�)
    std::vector<double> weight;                         //�e�A�C�e���̏d��(�������, �y�i���e�B�p)
    std::vector<double> values;                         //�e�A�C�e�������l(�ړI�֐��p)
    std::vector<std::vector<double>> item_relations;    //���ꂼ��̃A�C�e���Ԃ̊֌W�l  item_relation[type][item_id]
    std::vector<std::vector<double>> group_relations;   //�A�C�e���ƃO���[�v�̊Ԃ̊֌W�l  group_relation[type][group_id]
    std::vector<int> item_penalty;                      //���ꂼ��̃A�C�e���Ԃ̃y�i���e�B�l
    std::vector<int> group_penalty;                     //�A�C�e���ƃO���[�v�̊Ԃ̃y�i���e�B�l
};

#endif