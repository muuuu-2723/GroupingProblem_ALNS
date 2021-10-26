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
#include <fstream>
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
#include <Windows.h>
#include <iomanip>

using std::vector;

void solve(const Input& input, const std::filesystem::path& data_file, bool is_debug, int debug_num);

int main(int argc, char* argv[]) {
    bool is_debug = false;
    int debug_num;
    std::filesystem::path data_file("random_data.dat");
    std::filesystem::path problem_file("problem.dat");

    std::unique_ptr<Input> input;

    try {
        std::runtime_error argument_error("コマンドライン引数エラー : run.exe [-d] [-ip InputProblemFile] [-id InputDataFile]");
        if (argc > 6) {
            throw argument_error;
        }
        for (int i = 1; i < argc; ++i) {
            if (std::strcmp(argv[i], "-d") == 0) {
                is_debug = true;
            }
            else if (std::strcmp(argv[i], "-ip") == 0) {
                if (++i == argc) {
                    throw argument_error;
                }
                problem_file = argv[i];
            }
            else if (std::strcmp(argv[i], "-id") == 0) {
                if (++i == argc) {
                    throw argument_error;
                }
                data_file = argv[i];
            }
            else {
                throw argument_error;
            }
        }

        input = std::make_unique<Input>(problem_file, data_file);
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

    solve(*input, data_file, is_debug, debug_num);

    return 0;
}

void solve(const Input& input, const std::filesystem::path& data_file, bool is_debug, int debug_num) {
    double relation_ave = 0;
    double penalty_ave = 0;
    double ave_balance_ave = 0;
    double sum_balance_ave = 0;
    double group_num_ave = 0;
    double eval_ave = 0;
    double time_ave = 0;
    int N = 1;
    int M = 3000;

    for (int i = 0; i < N; i++) {
        vector<double> search_p, destroy_p;
        vector<std::string> color_map;
        std::ofstream eval_out, search_out, destroy_out;

        auto start = std::chrono::high_resolution_clock::now();
        auto now = std::make_unique<Solution>(input);
        now->evaluation_all(input.get_items());
        std::cerr << *now;
        Solution best(*now);
        /*auto t_begin = now.debug_evaluation_all(input.get_items());
        std::cout << std::get<2>(t_begin) << std::endl;*/

        vector<std::unique_ptr<Search>> searches;
        searches.emplace_back(std::make_unique<GroupPenaltyGreedy>(input.get_items(), 0, 1));
        searches.emplace_back(std::make_unique<ItemPenaltyGreedy>(input.get_items(), 0, 1));
        searches.emplace_back(std::make_unique<WeightPenaltyGreedy>(input.get_items(), 0, 1));
        searches.emplace_back(std::make_unique<RelationGreedy>(input.get_items(), 0, 1));
        searches.emplace_back(std::make_unique<ValueAverageGreedy>(input.get_items(), 0, 1));
        searches.emplace_back(std::make_unique<ValueSumGreedy>(input.get_items(), 0, 1));
        searches.emplace_back(std::make_unique<DecreaseGroup>(input.get_items(), 0, 1));
        searches.emplace_back(std::make_unique<ShiftNeighborhood>(input.get_items(), 1, 1));
        searches.emplace_back(std::make_unique<SwapNeighborhood>(input.get_items(), 1, /*4*/2));
        searches.emplace_back(std::make_unique<NeighborhoodGraph>(input.get_items(), 1, 4));
        searches.emplace_back(std::make_unique<ValueDiversityGreedy>(input.get_items(), 0, 1));

        std::shared_ptr<RandomDestroy> random_destroy = std::make_shared<RandomDestroy>(input.get_items(), (1.5 * Item::N) / Group::N, 1, 1);
        std::shared_ptr<MinimumDestroy> minimum_destroy = std::make_shared<MinimumDestroy>(input.get_items(), (1.5 * Item::N) / Group::N, 1, 1);

        vector<std::shared_ptr<Destroy>> destructions;
        destructions.emplace_back(random_destroy);
        destructions.emplace_back(std::make_shared<RandomGroupDestroy>(input.get_items(), Group::N / 3, 1, 1));
        destructions.emplace_back(minimum_destroy);
        destructions.emplace_back(std::make_shared<MinimumGroupDestroy>(input.get_items(), Group::N / 3, 1, 1));
        destructions.emplace_back(std::make_shared<Destroy>(input.get_items(), 1, 1));

        vector<double> search_weights(searches.size(), 1);
        vector<double> destroy_weights(destructions.size() - 1, 1);

        int cnt = 0;
        int best_change_cnt = cnt;
        vector<double> search_time(searches.size(), 0);
        vector<int> counter(searches.size(), 0);
        int moving_idx = -1;
        vector<vector<int>> score_cnt(searches.size(), vector<int>(4, 0));

        auto search_itr = searches.begin();
        std::generate(search_weights.begin(), search_weights.end(), [&search_itr]() { return (*search_itr++)->get_weight(); });
        DiscreteDistribution search_random(search_weights);
        auto destroy_itr = destructions.begin();
        std::generate(destroy_weights.begin(), destroy_weights.end(), [&destroy_itr]() { return (*destroy_itr++)->get_weight(); });
        DiscreteDistribution destroy_random(destroy_weights);

        std::unique_ptr<Debug> debug_ptr;
        if (is_debug) {
            double max_eval = 4000;
            debug_ptr = std::make_unique<Debug>(search_random, destroy_random, now, best, cnt, data_file.filename().string(), debug_num, M, max_eval, input);
        }

        while (cnt < M) {
            int search_idx = search_random();
            int destroy_idx = destructions.size() - 1;
            if (search_idx < 7) {
                destroy_idx = destroy_random();
            }
            else if (search_idx == 10) {
                do {
                    destroy_idx = destroy_random();
                } while (destroy_idx != 1 && destroy_idx != 3);
            }

            std::cerr << search_idx << " " << destroy_idx << std::endl;
            auto sstart = std::chrono::high_resolution_clock::now();
            auto next_solution = (*searches[search_idx])(*now, destructions[destroy_idx]);
            //std::cerr << *next_solution << std::endl;
            auto send = std::chrono::high_resolution_clock::now();
            double stime = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(send - sstart).count() / 1000.0);
            search_time[search_idx] += stime;
            ++counter[search_idx];
            double prev_now_eval, prev_best_eval;

            if (next_solution->get_eval_value() > best.get_eval_value() - std::abs(best.get_eval_value()) * 0.005 && random_destroy->get_destroy_num() > (0.5 * Item::N) / Group::N) {
                random_destroy->set_destroy_num((0.5 * Item::N) / Group::N);
                minimum_destroy->set_destroy_num((0.5 * Item::N) / Group::N);
            }

            double score;
            if (next_solution->get_eval_value() > best.get_eval_value()) {
                score = 100;
                now = std::move(next_solution);
                best = *now;
                best_change_cnt = cnt;
                if (cnt != 0 && random_destroy->get_destroy_num() > (0.5 * Item::N) / Group::N) {
                    random_destroy->set_destroy_num((0.5 * Item::N) / Group::N);
                }
                if (cnt != 0 && minimum_destroy->get_destroy_num() > (0.5 * Item::N) / Group::N) {
                    minimum_destroy->set_destroy_num((0.5 * Item::N) / Group::N);
                }
                /*if (moving_idx != -1) {
                    searches[moving_idx]->update_weight(score);
                    std::cerr << cnt << " " << moving_idx << std::endl;
                    moving_idx = -1;
                }*/
                score_cnt[search_idx][0]++;
            }
            else if (next_solution->get_eval_value() > now->get_eval_value()) {
                score = 60;
                now = std::move(next_solution);
                score_cnt[search_idx][1]++;
            }
            else if (next_solution->get_eval_value() < now->get_eval_value() && now->get_penalty() - next_solution->get_penalty() < 1) {
                score = 0.5;
                now = std::move(next_solution);
                moving_idx = destroy_idx;
                score_cnt[search_idx][2]++;
            }
            else {
                score = 0.1;
                score_cnt[search_idx][3]++;
            }

            std::cerr << score << std::endl;

            searches[search_idx]->update_weight(score);
            //search_weights[search_idx] = search_weights[search_idx] * lambda + score * (1 - lambda);
            if (destroy_idx < destroy_weights.size()) {
                destructions[destroy_idx]->update_weight(score);
                //destroy_weights[destroy_idx] = destroy_weights[destroy_idx] * lambda + score * (1 - lambda);
            }
            search_itr = searches.begin();
            std::generate(search_weights.begin(), search_weights.end(), [&search_itr]() { std::cerr << (*search_itr)->get_weight() << " "; return (*search_itr++)->get_weight(); });
            std::cerr << std::endl;
            search_random.set_weight(search_weights);
            destroy_itr = destructions.begin();
            std::generate(destroy_weights.begin(), destroy_weights.end(), [&destroy_itr]() { return (*destroy_itr++)->get_weight(); });
            destroy_random.set_weight(destroy_weights);

            if (is_debug) {
                debug_ptr->output();
            }

            if (cnt % (M / 100) == 0) {
                random_destroy->add_destroy_num(-1);
                minimum_destroy->add_destroy_num(-1);
            }
            if ((cnt - best_change_cnt) % (int)((M / 100) * 0.75 * (Item::N / Group::N)) == 0 && cnt != best_change_cnt) {
                random_destroy->add_destroy_num(Item::N / Group::N);
                minimum_destroy->add_destroy_num(Item::N / Group::N);
            }

            ++cnt;
            //if (cnt > M / 3 && cnt > best_change_cnt * 2) break;
        }

        for (int j = 0; j < searches.size(); ++j) {
            std::cerr << typeid(*searches[j]).name() << ":" << search_weights[j] << std::endl;
            std::cerr << typeid(*searches[j]).name() << ":" << search_time[j] / counter[j] << "[ms]" << std::endl;
            for (auto&& s_cnt : score_cnt[j]) std::cerr << (double)s_cnt / counter[j] * 100 << " ";
            std::cerr << std::endl;
        }

        for (int j = 0; j < destroy_weights.size(); ++j) {
            std::cerr << typeid(*destructions[j]).name() << ":" << destroy_weights[j] << std::endl;
        }

        auto end = std::chrono::high_resolution_clock::now();
        double time = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0);
    
        time_ave += time;
        std::cout << best;
        std::cout << "time = " << time << "[ms]" << std::endl;
        std::cout << "cnt = " << cnt << std::endl;
        /*std::cout << "eval = " << now.get_eval_value() << std::endl;
        std::cout << "penalty = " << now.get_debug_penalty() << std::endl;
        std::cout << "deviation = " << std::sqrt(now.get_debug_dispersion()) << std::endl;*/
        std::cerr << "relation:" << best.get_relation() << std::endl;
        std::cerr << "penalty:" << best.get_penalty() << std::endl;
        std::cerr << "ave_balance:" << best.get_ave_balance() << std::endl;
        std::cerr << "sum_balance:" << best.get_sum_balance() << std::endl;
        std::cerr << "group_num:" << best.get_valid_groups().size() << std::endl;
        std::cerr << "eval:" << best.get_eval_value() << std::endl;
        best.evaluation_all(input.get_items());
        relation_ave += best.get_relation();
        penalty_ave += best.get_penalty();
        ave_balance_ave += best.get_ave_balance();
        sum_balance_ave += best.get_sum_balance();
        group_num_ave += best.get_valid_groups().size();
        eval_ave += best.get_eval_value();
        /*for (const auto& group : now.get_groups()) {
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 2; j++) {
                    for (int k = 0; k < 2; k++) {
                        std::cout << group.get_num_ratio(i, j, k) << " ";
                    }
                }
            }
            std::cout << std::endl;
        }
        std::cout << now;*/
    }
    std::cout << "relation_ave:" << relation_ave / N << std::endl;
    std::cout << "penalty_ave:" << penalty_ave / N << std::endl;
    std::cout << "ave_balance_ave:" << ave_balance_ave / N << std::endl;
    std::cout << "sum_balance_ave:" << sum_balance_ave / N << std::endl;
    std::cout << "group_num_ave:" << group_num_ave / N << std::endl;
    std::cout << "eval_ave:" << eval_ave / N << std::endl;
    std::cout << "time_ave:" << time_ave / N << "[ms]" << std::endl;
}