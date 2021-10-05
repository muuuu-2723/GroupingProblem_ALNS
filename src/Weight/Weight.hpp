#ifndef WEIGHT_H_
#define WEIGHT_H_

class Weight {
private:
    static double lambda;
    double weight;
    int param;
public:
    Weight(double init_weight, int param) : weight(init_weight), param(param) {}
    void update(double score);
    double get_weight() const;
};

inline double Weight::get_weight() const {
    return weight;
}

inline void Weight::update(double score) {
    weight = lambda * weight + (1 - lambda) * score / param;
}

#endif