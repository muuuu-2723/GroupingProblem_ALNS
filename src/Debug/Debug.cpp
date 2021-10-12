#include "Debug.hpp"
#include <matplotlib.hpp>
#include <Solution.hpp>
#include <MyRandom.hpp>
#include <Input.hpp>
#include <vector>
#include <filesystem>
#include <string>
#include <iostream>
#include <iomanip>
#include <Windows.h>

using std::vector;
using std::string;

Debug::Debug(const DiscreteDistribution& search_random, const DiscreteDistribution& destroy_random, const Solution& solution,
             const Solution& best, const int& x, const std::string& datafile, int debug_num, int max_x, double max_eval)
             : search_random(search_random), destroy_random(destroy_random), solution(solution), best(best), x(x), debug_num(debug_num) {
    
    auto output_dir = Input::get_exe_path().parent_path();
    eval_out.open(output_dir / ("eval_" + datafile));
    search_out.open(output_dir / ("search_" + datafile));
    destroy_out.open(output_dir / ("destroy_" + datafile));

    if (this->debug_num != 0) {
        if (!g.open()) {
            this->debug_num = 0;
            std::cerr << "graph output is error" << std::endl;
        }
    }

    color_map = {"turquoise", "orange", "green", "tomato", "orchid", "sienna", "fuchsia", "grey", "gold", "cyan"};

    if (this->debug_num == 1) {
        g.screen(0, 0, max_x, max_eval);
    }
    else if (this->debug_num != 0) {
        g.screen(0, 0, max_x, 100);
        lower_eval = max_eval - 60;
    }

    if (this->debug_num != 0) {
        prev_eval = solution.get_eval_value();
        prev_best_eval = best.get_eval_value();
    }
    if (this->debug_num == 2) {
        prev_p = search_random.get_probabilities();
    }
    else if (this->debug_num == 3) {
        prev_p = destroy_random.get_probabilities();
    }

    eval_out << std::fixed << std::setprecision(8) << this->x << " " << prev_eval << " " << prev_best_eval << std::endl;

    search_out << this->x;
    for (auto&& p : search_random.get_probabilities()) {
        search_out << " " << p * 100;
    }
    search_out << std::endl;

    destroy_out << this->x;
    for (auto&& p : destroy_random.get_probabilities()) {
        destroy_out << " " << p * 100;
    }
    destroy_out << std::endl;
}

void Debug::output() {
    eval_out << x + 1 << " " << solution.get_eval_value() << " " << best.get_eval_value() << std::endl;
    search_out << x + 1;
    for (auto&& p : search_random.get_probabilities()) {
        search_out << " " << p * 100;
    }
    search_out << std::endl;
    destroy_out << x + 1;
    for (auto&& p : destroy_random.get_probabilities()) {
        destroy_out << " " << p * 100;
    }
    destroy_out << std::endl;

    if (debug_num == 1) {
        g.line(x, prev_eval, x + 1, solution.get_eval_value(), "lightgreen");
        g.line(x, prev_best_eval, x + 1, best.get_eval_value(), "red");
    }
    else if (debug_num != 0) {
        g.line(x, prev_eval - lower_eval, x + 1, solution.get_eval_value() - lower_eval, "lightgreen");
        g.line(x, prev_best_eval - lower_eval, x + 1, best.get_eval_value() - lower_eval, "red");
    }

    if (debug_num == 2) {
        auto now_p = search_random.get_probabilities();
        for (size_t i = 0, size = now_p.size(); i < size; ++i) {
            g.line(x, prev_p[i] * 100, x + 1, now_p[i] * 100, color_map[i % color_map.size()].c_str());
        }
        prev_p = std::move(now_p);
    }
    if (debug_num == 3) {
        auto now_p = destroy_random.get_probabilities();
        for (size_t i = 0, size = now_p.size(); i < size; ++i) {
            g.line(x, prev_p[i] * 100, x + 1, now_p[i] * 100, color_map[i % color_map.size()].c_str());
        }
        prev_p = std::move(now_p);
    }
    if (debug_num != 0) {
        prev_eval = solution.get_eval_value();
        prev_best_eval = best.get_eval_value();
    }
}