#ifndef MYRANDOM_H_
#define MYRANDOM_H_

#include <random>
#include <algorithm>

/*�����֘A���܂Ƃ߂�*/
class MyRandom {
protected:
    static std::mt19937_64 mt;

public:
    /*�����_���A�N�Z�X�̃R���e�i���V���b�t��*/
    template<class RanContainer>
    static void shuffle(RanContainer& container) {
        std::shuffle(container.begin(), container.end(), mt);
    }
};


/*�w��͈�(min�ȏ�max�ȉ�)�̈�l���z�̐���*/
template<class IntType = int>
class RandomInt : public MyRandom {
private:
    std::uniform_int_distribution<IntType> dist;

public:
    RandomInt(IntType min, IntType max) 
        : dist(std::uniform_int_distribution<IntType> (min, max)) {}

    /*��������*/
    IntType operator()() {
        return dist(mt);
    }
};


/*�w��͈�(min�ȏ�max�ȉ�)�̈�l���z�̎���*/
template<class RealType = double>
class RandomReal : public MyRandom {
private:
    std::uniform_real_distribution<RealType> dist;

public:
    RandomReal(RealType min, RealType max)
        : dist(std::uniform_real_distribution<RealType> (min, max)) {}
    
    /*��������*/
    RealType operator()() {
        return dist(mt);
    }
};

/*�d�݂Ɋ�Â������𐶐�*/
class DiscreteDistribution : public MyRandom {
private:
    std::discrete_distribution<> dist;
public:
    DiscreteDistribution(std::vector<double>& probabilities)
        : dist(probabilities.begin(), probabilities.end()) {}
    
    /*�m�����z�Ɋ�Â��C���f�b�N�X�𐶐�*/
    int operator()() {
        return dist(mt);
    }

    /*���ۂ̊m�����擾*/
    std::vector<double> get_probabilities() {
        return dist.probabilities();
    }
};

#endif