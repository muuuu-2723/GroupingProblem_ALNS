#include "Debug.hpp"
#include <matplotlib.hpp>
#include <Solution.hpp>
#include <MyRandom.hpp>
#include <Input.hpp>
#include <Search.hpp>
#include <vector>
#include <filesystem>
#include <string>
#include <iostream>
#include <iomanip>
#include <typeinfo>
#include <Windows.h>

using std::vector;
using std::string;

Debug::Debug(const DiscreteDistribution& search_random, const vector<std::unique_ptr<Search>>& searches, const std::unique_ptr<Solution>& solution,
             const Solution& best, const int& x, const std::string& problem_file, int debug_num, int max_x, double max_eval, const Input& input)
             : search_random(search_random), searches(searches), solution(solution), best(best), x(x), debug_num(debug_num) {
    
    auto output_dir = Input::get_exe_path().parent_path();
    auto data_file = std::filesystem::path(problem_file).stem();
    output_dir /= data_file.string();
    try {
        std::filesystem::create_directory(output_dir);    
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        exit(1);
    }
    
    eval_out.open(output_dir / "eval.dat");
    search_out.open(output_dir / "search.dat");
    destroy_outs.resize(searches.size());
    for (size_t i = 0, size = searches.size(); i < size; ++i) {
        if (!searches[i]->get_destroy_random()) continue;
        destroy_outs[i].open(output_dir / (std::string(typeid(*searches[i]).name()) + std::string("_destroy.dat")));
    }
    item_times_path = output_dir / "item_times.csv";
    group_times_path = output_dir / "group_times.csv";

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
            std::string fig_file = "fig.pdf";
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
    /*else if (this->debug_num == 3) {
        prev_p = destroy_random.get_probabilities();
    }*/

    eval_out << std::fixed << std::setprecision(8) << this->x << " " << prev_eval << " " << prev_best_eval << std::endl;

    search_out << this->x;
    for (auto&& p : search_random.get_probabilities()) {
        search_out << " " << p * 100;
    }
    search_out << std::endl;

    for (size_t i = 0, size = searches.size(); i < size; ++i) {
        if (!searches[i]->get_destroy_random()) continue;
        destroy_outs[i] << this->x;
        for (auto&& p : searches[i]->get_destroy_random()->get_probabilities()) {
            destroy_outs[i] << " " << p * 100;
        }
        destroy_outs[i] << std::endl;
    }
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
    for (auto&& out : destroy_outs) {
        out.close();
    }

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

    for (size_t i = 0, size = searches.size(); i < size; ++i) {
        if (!searches[i]->get_destroy_random()) continue;
        destroy_outs[i] << x + 1;
        for (auto&& p : searches[i]->get_destroy_random()->get_probabilities()) {
            destroy_outs[i] << " " << p * 100;
        }
        destroy_outs[i] << std::endl;
    }

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
    /*if (debug_num == 3) {
        auto now_p = destroy_random.get_probabilities();
        for (size_t i = 0, size = now_p.size(); i < size; ++i) {
            g.line(x, prev_p[i] * 100, x + 1, now_p[i] * 100, color_map[i % color_map.size()].c_str());
        }
        prev_p = std::move(now_p);
    }*/
    if (debug_num != 0) {
        prev_eval = solution->get_eval_value() * param;
        prev_best_eval = best.get_eval_value() * param;
    }
    solution->counter();
}