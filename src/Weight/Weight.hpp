#ifndef WEIGHT_H_
#define WEIGHT_H_

/*�j��@�ƏC���@�ƋߖT�̑I���m�������߂�d��*/
class Weight {
private:
    static double lambda;                       //1��̒T���ł̕ω���. 0�`1�ő傫���قǕω����Ȃ�
    double weight;                              //�d��
    int param;                                  //��@���Ƃ̕ω�����ω�������p�����[�^. �傫���قǕω����Ȃ�
public:
    /*�R���X�g���N�^*/
    Weight(double init_weight, int param) : weight(init_weight), param(param) {}
    void update(double score);                  //�d�݂�score�Ɋ�Â��ĕω�������
    double get_weight() const;                  //�d�݂��擾
};

/*�d�݂��擾*/
inline double Weight::get_weight() const {
    return weight;
}

/*�d�݂�score�Ɋ�Â��ĕω�������*/
inline void Weight::update(double score) {
    //weight = lambda * weight + (1 - lambda) * score / param;
    weight += score / param;
    if (weight < 1) weight = 1;
    if (weight > 100) weight = 100;
}

#endif