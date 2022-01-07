#ifndef WEIGHT_H_
#define WEIGHT_H_

/*破壊法と修復法と近傍の選択確率を決める重み*/
class Weight {
private:
    static double lambda;                       //1回の探索での変化率. 0～1で大きいほど変化しない
    double weight;                              //重み
    int param;                                  //手法ごとの変化率を変化させるパラメータ. 大きいほど変化しない
public:
    /*コンストラクタ*/
    Weight(double init_weight, int param) : weight(init_weight), param(param) {}
    void update(double score);                  //重みをscoreに基づいて変化させる
    double get_weight() const;                  //重みを取得
};

/*重みを取得*/
inline double Weight::get_weight() const {
    return weight;
}

/*重みをscoreに基づいて変化させる*/
inline void Weight::update(double score) {
    //weight = lambda * weight + (1 - lambda) * score / param;
    weight += score / param;
    if (weight < 1) weight = 1;
    if (weight > 100) weight = 100;
}

#endif