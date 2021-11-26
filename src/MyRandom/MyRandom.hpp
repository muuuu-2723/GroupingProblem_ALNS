#ifndef MYRANDOM_H_
#define MYRANDOM_H_

#include <random>
#include <algorithm>

/*乱数関連をまとめる*/
class MyRandom {
protected:
    static std::mt19937_64 mt;

public:
    /*ランダムアクセスのコンテナをシャッフル*/
    template<class RanContainer>
    static void shuffle(RanContainer& container) {
        std::shuffle(container.begin(), container.end(), mt);
    }
    /*サンプルを抽出*/
    template<class InputContainer, class OutputContainer>
    static void sample(InputContainer& input, OutputContainer& output, size_t size) {
        std::sample(input.begin(), input.end(), std::back_inserter(output), size, mt);
    }
};


/*指定範囲(min以上max以下)の一様分布の整数*/
template<class IntType = int>
class RandomInt : public MyRandom {
private:
    std::uniform_int_distribution<IntType> dist;

public:
    RandomInt(IntType min, IntType max) 
        : dist(std::uniform_int_distribution<IntType> (min, max)) {}

    /*乱数生成*/
    IntType operator()() {
        return dist(mt);
    }
};


/*指定範囲(min以上max以下)の一様分布の実数*/
template<class RealType = double>
class RandomReal : public MyRandom {
private:
    std::uniform_real_distribution<RealType> dist;

public:
    RandomReal(RealType min, RealType max)
        : dist(std::uniform_real_distribution<RealType> (min, max)) {}
    
    /*乱数生成*/
    RealType operator()() {
        return dist(mt);
    }
};

/*重みに基づく乱数を生成*/
class DiscreteDistribution : public MyRandom {
private:
    std::discrete_distribution<> dist;
public:
    DiscreteDistribution(const std::vector<double>& probabilities)
        : dist(probabilities.begin(), probabilities.end()) {}
    
    /*確率分布に基づくインデックスを生成*/
    int operator()() {
        return dist(mt);
    }

    /*重みの設定*/
    void set_weight(const std::vector<double>& probabilities) {
        std::discrete_distribution<>::param_type param(probabilities.begin(), probabilities.end());
        dist.param(param);
    }

    /*実際の確率を取得*/
    std::vector<double> get_probabilities() const {
        return dist.probabilities();
    }
};

#endif