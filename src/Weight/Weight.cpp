#include "Weight.hpp"

double Weight::lambda = 0.999;

void Weight::update(double score) {
    //++cnt;
    //weight = weight + pow_lambda * (1 - score) * (lambda - 1) / param;
    weight = lambda * weight + (1 - lambda) * score / param;
    //if (cnt % param == 0) pow_lambda *= lambda;
}