#include "MyRandom.hpp"

namespace {
    std::random_device rnd;
}

/*static変数の初期化*/
std::mt19937_64 MyRandom::mt(rnd());