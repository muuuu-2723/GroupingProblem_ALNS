#include "MyRandom.hpp"

namespace {
    std::random_device rnd;
}

/*static�ϐ��̏�����*/
std::mt19937_64 MyRandom::mt(10);