#ifndef WEIGHT_H_
#define WEIGHT_H_

class Weight {
private:
    static double lambda;
    double weight;
    int cnt;
    int param;
    double pow_lambda;
public:
    Weight(int param) : weight(1), cnt(0), param(param), pow_lambda(1) {}
    void update(double score);
    double get_weight() const;
};

inline double Weight::get_weight() const {
    return weight;
}

#endif