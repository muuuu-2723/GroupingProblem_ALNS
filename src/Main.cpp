#include <Item.hpp>
#include <Group.hpp>
#include <Solution.hpp>
#include <MyRandom.hpp>
#include <Search.hpp>
#include <Destroy.hpp>
#include <Debug.hpp>
#include <Input.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <chrono>
#include <memory>
#include <cmath>
#include <iterator>
#include <typeinfo>
#include <cstring>
#include <filesystem>
#include <iomanip>
#include <sstream>

using std::vector;

void solve(const Input& input, const std::filesystem::path& data_file, bool is_debug, int debug_num, const std::string& add_output_name);

int main(int argc, char* argv[]) {
    auto cp = std::filesystem::current_path();
    bool is_debug = false;
    int debug_num = -1;
    std::filesystem::path problem_file("bin_Falkenauer_t120_00.json");
    std::string add_output_name;

    std::unique_ptr<Input> input;

    try {
        std::runtime_error argument_error("コマンドライン引数エラー : run.exe [-p InputProblemFile]");
        if (argc > 6 || argc < 1) {
            throw argument_error;
        }
        auto exe_path = cp / argv[0];
        if (!std::filesystem::exists(exe_path)) {
            throw argument_error;
        }
        for (int i = 1; i < argc; ++i) {
            if (std::strcmp(argv[i], "-d") == 0) {
                is_debug = true;
            }
            else if (std::strcmp(argv[i], "-p") == 0) {
                if (++i == argc) {
                    throw argument_error;
                }
                problem_file = argv[i];
            }
            else if (std::strcmp(argv[i], "-o") == 0) {
                if (++i == argc) {
                    throw argument_error;
                }
                add_output_name = argv[i];
            }
            else {
                throw argument_error;
            }
        }

        input = std::make_unique<Input>(problem_file, exe_path);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }
    
    if (is_debug) {
        do {
            std::cout << "リアルタイムグラフで描画するものを選択" << std::endl;
            std::cout << "0:なし, 1:評価値, 2:構築法の確率, 3:破壊法の確率" << std::endl;
            std::cout << "debug_num = ";
            std::cin >> debug_num;
        } while (debug_num < 0 || debug_num > 3);
    }

    solve(*input, problem_file, is_debug, debug_num, add_output_name);

    return 0;
}

void solve(const Input& input, const std::filesystem::path& problem_file, bool is_debug, int debug_num, const std::string& add_output_name) {
    double relation_ave = 0;
    double penalty_ave = 0;
    double ave_balance_ave = 0;
    double sum_balance_ave = 0;
    double group_num_ave = 0;
    double eval_ave = 0;
    double time_ave = 0;
    int N = 1;
    int M = 5000;

    for (int i = 0; i < N; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        auto now = std::make_unique<Solution>(input);
        now->evaluation_all(input.get_items());
        Solution best(*now);

        vector<std::unique_ptr<Search>> searches;
        searches.emplace_back(std::make_unique<GroupPenaltyGreedy>(input.get_items(), 1, 1, *now));
        searches.emplace_back(std::make_unique<ItemPenaltyGreedy>(input.get_items(), 1, 1, *now));
        searches.emplace_back(std::make_unique<WeightPenaltyGreedy>(input.get_items(), 1, 1, *now));
        searches.emplace_back(std::make_unique<RelationGreedy>(input.get_items(), 1, 1, *now));
        searches.emplace_back(std::make_unique<ValueAverageGreedy>(input.get_items(), 1, 1, *now));
        searches.emplace_back(std::make_unique<ValueSumGreedy>(input.get_items(), 1, 1, *now));
        searches.emplace_back(std::make_unique<DecreaseGroup>(input.get_items(), 1, 1, *now));
        searches.emplace_back(std::make_unique<ShiftNeighborhood>(input.get_items(), 1, 1, *now));
        searches.emplace_back(std::make_unique<SwapNeighborhood>(input.get_items(), 1, /*4*/1, *now));
        searches.emplace_back(std::make_unique<NeighborhoodGraph>(input.get_items(), 1, 4, *now));
        searches.emplace_back(std::make_unique<ValueDiversityGreedy>(input.get_items(), 1, 1, *now));

        vector<double> search_weights(searches.size(), 1);

        int cnt = 0;
        int best_change_cnt = cnt;
        vector<double> search_time(searches.size(), 0);
        vector<int> counter_search(searches.size(), 0);
        int moving_idx = -1;

        auto search_itr = searches.begin();
        std::generate(search_weights.begin(), search_weights.end(), [&search_itr]() { return (*search_itr++)->get_weight(); });
        DiscreteDistribution search_random(search_weights);

        std::unique_ptr<Debug> debug_ptr;
        if (is_debug) {
            double max_eval = 350000;
            std::stringstream ss;
            ss << "_" << i;
            debug_ptr = std::make_unique<Debug>(search_random, searches, now, best, cnt, problem_file.filename().stem().string() + add_output_name + ss.str(), debug_num, M, max_eval, input);
        }
        bool intensification = true;
        int diversification_cnt = 0;
        Solution searched_solution(*now);

        while (cnt < M) {
            int search_idx = search_random();

            auto next_solution = (*searches[search_idx])(*now);
            auto send = std::chrono::high_resolution_clock::now();

            if (next_solution->get_eval_value() > best.get_eval_value()/* - std::abs(best.get_eval_value()) * 0.0005*/) {
                for (auto&& s : searches) {
                    s->reset_destroy_num(*next_solution);
                }
                intensification = true;
            }
            double score;
            if (next_solution->get_eval_value() > best.get_eval_value()) {
                score = 100;
                now = std::move(next_solution);
                best = *now;
                best_change_cnt = cnt;
            }
            else if (next_solution->get_eval_value() >= now->get_eval_value()) {
                if (searches[search_idx]->get_is_move()) {
                    score = 30;
                    now = std::move(next_solution);
                }
                else {
                    score = 0.1;
                }
            }
            else if (now->get_penalty() >= next_solution->get_penalty() - 1) {
                score = 5;
                now = std::move(next_solution);
            }
            else {
                score = 0.1;
            }

            searches[search_idx]->update_weight(score);
            search_itr = searches.begin();
            std::generate(search_weights.begin(), search_weights.end(), [&search_itr]() { return (*search_itr++)->get_weight(); });
            search_random.set_weight(search_weights);

            if (is_debug) {
                debug_ptr->output();
            }
            int d = distance(searched_solution, *now);
            std::cerr << "d = " << d << std::endl;
            //std::cerr << "size = " << now->debug_same_group() << ", p = " << now->get_penalty() << std::endl;

            //解の距離が20より大きくなったら(充分多様化されたら), 集中化に移行する
            if (!intensification && d > 10000) {
                std::cerr << "集中化2" << std::endl;
                std::cerr << searched_solution << std::endl;
                std::cerr << *now << std::endl;
                intensification = true;
                diversification_cnt = cnt;
            }

            //現在の解周辺を充分探索できたら, 多様化に移行する
            if (intensification && cnt - best_change_cnt > 200 && cnt - diversification_cnt > 200) {
                std::cerr << "多様化" << std::endl;
                intensification = false;
                searched_solution = *now;
            }
            if ((cnt - best_change_cnt) % (int)((M / 100) * 0.75 * (Item::N / Group::N)) == 0 && cnt != best_change_cnt) {
                for (auto&& s : searches) {
                    s->update_destroy_num(*now, false);
                }
            }
            ++cnt;
        }

        auto end = std::chrono::high_resolution_clock::now();
        double time = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0);
    
        std::cout << best;
        std::cout << "time = " << time << "[ms]" << std::endl;
    }
}