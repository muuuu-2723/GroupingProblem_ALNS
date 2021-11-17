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

Debug::Debug(const DiscreteDistribution& search_random, const DiscreteDistribution& destroy_random, const std::unique_ptr<Solution>& solution,
             const Solution& best, const int& x, const std::string& problem_file, int debug_num, int max_x, double max_eval, const Input& input)
             : search_random(search_random), destroy_random(destroy_random), solution(solution), best(best), x(x), debug_num(debug_num) {
    
    auto output_dir = Input::get_exe_path().parent_path();
    auto data_file = std::filesystem::path(problem_file).replace_extension(".dat");
    eval_out.open(output_dir / ("eval_" + data_file.string()));
    search_out.open(output_dir / ("search_" + data_file.string()));
    destroy_out.open(output_dir / ("destroy_" + data_file.string()));
    item_times_path = output_dir / ("item_times_" + data_file.stem().string() + ".csv");
    group_times_path = output_dir / ("group_times_" + data_file.stem().string() + ".csv");

    if (input.get_opt() == Input::Opt::MAX) {
        param = 1;
    }
    else {
        param = -1;
    }

    if (this->debug_num != 0) {
        if (!g.open()) {
            this->debug_num = 0;
            std::cerr << "graph output is error" << std::endl;
        }
        else {
            std::string fig_file = "fig_" + std::filesystem::path(problem_file).stem().string() + ".pdf";
            auto current_path = std::filesystem::current_path();
            fig_file_path = output_dir.lexically_relative(current_path).append(fig_file).generic_string();
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
        prev_eval = solution->get_eval_value() * param;
        prev_best_eval = best.get_eval_value() * param;
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

Debug::~Debug() {
    std::ofstream item_times_out(item_times_path);
    for (auto&& vec : solution->get_item_times()) {
        for (auto&& count : vec) {
            item_times_out << count << ",";
        }
        item_times_out << std::endl;
    }
    item_times_out.close();
    std::ofstream group_times_out(group_times_path);
    for (auto&& vec : solution->get_group_times()) {
        for (auto&& count : vec) {
            group_times_out << count << ",";
        }
        group_times_out << std::endl;
    }
    group_times_out.close();
    eval_out.close();
    search_out.close();
    destroy_out.close();

    if (debug_num != 0) {
        g.save(fig_file_path.c_str());
        g.close();
    }
}

void Debug::output() {
    eval_out << x + 1 << " " << solution->get_eval_value() * param << " " << best.get_eval_value() * param << std::endl;
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
        g.line(x, prev_eval, x + 1, solution->get_eval_value() * param, "lightgreen");
        g.line(x, prev_best_eval, x + 1, best.get_eval_value() * param, "red");
    }
    else if (debug_num != 0) {
        g.line(x, prev_eval - lower_eval, x + 1, solution->get_eval_value() * param - lower_eval, "lightgreen");
        g.line(x, prev_best_eval - lower_eval, x + 1, best.get_eval_value() * param - lower_eval, "red");
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
        prev_eval = solution->get_eval_value() * param;
        prev_best_eval = best.get_eval_value() * param;
    }
    solution->counter();
}