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

vector<Item> items;
void input(const std::filesystem::path& file_name);

int main(int argc, char* argv[]) {
    double profit_ave = 0;
    double penalty_ave = 0;
    double deviation_ave = 0;
    double eval_ave = 0;
    double time_ave = 0;
    int N = 1;
    int M = 30000;
    bool is_debug = false;
    int debug_num;
    std::filesystem::path datafile("random_data.dat");

    if (argc > 3) {
        std::cerr << "引数エラー" << std::endl;
        std::cerr << "run.exe [-d] [inputfile]" << std::endl;
        exit(1);
    }
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-d") == 0) {
            is_debug = true;
        }
        else {
            datafile = argv[i];
        }
    }
    if (is_debug) {
        std::cout << "1:評価値, 2:構築法の確率, 3:破壊法の確率" << std::endl;
        std::cout << "debug_num = ";
        std::cin >> debug_num;
    }

    auto data_dir = Input::get_exe_path().parent_path().parent_path().append("Data");
    auto input_file_path = data_dir / datafile;
    if (!std::filesystem::exists(input_file_path)) {
        input_file_path = datafile;
        datafile = datafile.filename();
    }
    std::filesystem::path output_dir;
    for (int i = 0; i < N; i++) {
        input(input_file_path);
        vector<double> search_p, destroy_p;
        vector<std::string> color_map;
        std::ofstream eval_out, search_out, destroy_out;

        auto start = std::chrono::high_resolution_clock::now();
        Solution now(items);
        now.evaluation_all(items);
        std::cerr << now;
        Solution best(now);
        /*auto t_begin = now.debug_evaluation_all(items);
        std::cout << std::get<2>(t_begin) << std::endl;*/

        vector<std::unique_ptr<Search>> searches;
        searches.emplace_back(std::make_unique<GroupPenaltyGreedy>(items, 1, 1));
        searches.emplace_back(std::make_unique<ItemPenaltyGreedy>(items, 1, 1));
        searches.emplace_back(std::make_unique<WeightPenaltyGreedy>(items, 1, 1));
        searches.emplace_back(std::make_unique<RelationGreedy>(items, 1, 1));
        searches.emplace_back(std::make_unique<ValueAverageGreedy>(items, 1, 1));
        searches.emplace_back(std::make_unique<ValueSumGreedy>(items, 1, 1));
        searches.emplace_back(std::make_unique<DecreaseGroup>(items, 1, 1));
        searches.emplace_back(std::make_unique<ShiftNeighborhood>(items, 1, 1));
        searches.emplace_back(std::make_unique<SwapNeighborhood>(items, 1, /*4*/2));
        searches.emplace_back(std::make_unique<NeighborhoodGraph>(items, 1, 4));
        searches.emplace_back(std::make_unique<ValueDiversityGreedy>(items, 1, 1));

        std::shared_ptr<RandomDestroy> random_destroy = std::make_shared<RandomDestroy>(items, (1.5 * Item::N) / Group::N, 1, 1);
        std::shared_ptr<MinimumDestroy> minimum_destroy = std::make_shared<MinimumDestroy>(items, (1.5 * Item::N) / Group::N, 1, 1);

        vector<std::shared_ptr<Destroy>> destructions;
        destructions.emplace_back(random_destroy);
        destructions.emplace_back(std::make_shared<RandomGroupDestroy>(items, Group::N / 3, 1, 1));
        destructions.emplace_back(minimum_destroy);
        destructions.emplace_back(std::make_shared<MinimumGroupDestroy>(items, Group::N / 3, 1, 1));
        /*destructions.emplace_back(std::make_shared<RandomDestroy>(items, Item::N / Group::N));
        destructions.emplace_back(std::make_shared<MinimumDestroy>(items, Item::N / Group::N));*/
        destructions.emplace_back(std::make_shared<Destroy>(items, 1, 1));

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
            debug_ptr = std::make_unique<Debug>(search_random, destroy_random, now, best, cnt, datafile, debug_num, M, max_eval);
        }

        while (cnt < M) {
            int search_idx = search_random();
            int destroy_idx = destructions.size() - 1;
            if (search_idx < 2) {
                destroy_idx = destroy_random();
            }
            else if (search_idx == 5) {
                do {
                    destroy_idx = destroy_random();
                } while (destroy_idx != 1 && destroy_idx != 3);
            }

            //std::cerr << search_idx << std::endl;
            auto sstart = std::chrono::high_resolution_clock::now();
            Solution next_solution = (*searches[search_idx])(now, destructions[destroy_idx]);
            auto send = std::chrono::high_resolution_clock::now();
            double stime = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(send - sstart).count() / 1000.0);
            search_time[search_idx] += stime;
            ++counter[search_idx];
            double prev_now_eval, prev_best_eval;

            if (next_solution.get_eval_value() > best.get_eval_value() - std::abs(best.get_eval_value()) * 0.005 && random_destroy->get_destroy_num() > (0.5 * Item::N) / Group::N) {
                random_destroy->set_destroy_num((0.5 * Item::N) / Group::N);
                minimum_destroy->set_destroy_num((0.5 * Item::N) / Group::N);
            }

            double score;
            if (next_solution.get_eval_value() > best.get_eval_value()) {
                score = 100;
                now = std::move(next_solution);
                best = now;
                best_change_cnt = cnt;
                if (cnt != 0 && random_destroy->get_destroy_num() > (0.5 * Item::N) / Group::N) {
                    random_destroy->set_destroy_num((0.5 * Item::N) / Group::N);
                }
                if (cnt != 0 && minimum_destroy->get_destroy_num() > (0.5 * Item::N) / Group::N) {
                    minimum_destroy->set_destroy_num((0.5 * Item::N) / Group::N);
                }
                if (moving_idx != -1) {
                    searches[moving_idx]->update_weight(score);
                    std::cerr << cnt << " " << moving_idx << std::endl;
                    moving_idx = -1;
                }
                score_cnt[search_idx][0]++;
            }
            else if (next_solution.get_eval_value() > now.get_eval_value()) {
                score = 60;
                now = std::move(next_solution);
                score_cnt[search_idx][1]++;
            }
            else if (next_solution.get_eval_value() < now.get_eval_value() && now.get_penalty() - next_solution.get_penalty() < 1) {
                score = 0.5;
                now = std::move(next_solution);
                moving_idx = destroy_idx;
                score_cnt[search_idx][2]++;
            }
            else {
                score = 0.1;
                score_cnt[search_idx][3]++;
            }

            searches[search_idx]->update_weight(score);
            //search_weights[search_idx] = search_weights[search_idx] * lambda + score * (1 - lambda);
            if (destroy_idx < destroy_weights.size()) {
                destructions[destroy_idx]->update_weight(score);
                //destroy_weights[destroy_idx] = destroy_weights[destroy_idx] * lambda + score * (1 - lambda);
            }
            search_itr = searches.begin();
            std::generate(search_weights.begin(), search_weights.end(), [&search_itr]() { return (*search_itr++)->get_weight(); });
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
        best.evaluation_all(items);
        profit_ave += best.get_relation();
        penalty_ave += best.get_penalty();
        deviation_ave += best.get_deviation();
        eval_ave += best.get_eval_value();
        if (is_debug && debug_num != 0) {
            std::string output_fig = "fig_" + datafile.stem().string() + ".pdf";
            auto current_path = std::filesystem::current_path();
            g.save(output_dir.lexically_relative(current_path).append(output_fig).generic_string().c_str());
            g.close();
        }
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
    std::cout << "profit_ave:" << profit_ave / N << std::endl;
    std::cout << "penalty_ave:" << penalty_ave / N << std::endl;
    std::cout << "deviation_ave:" << deviation_ave / N << std::endl;
    std::cout << "eval_ave:" << eval_ave / N << std::endl;
    std::cout << "time_ave:" << time_ave / N << "[ms]" << std::endl;

    return 0;
}

void input(const std::filesystem::path& file_name) {
    std::ifstream ifs(file_name);
    if (!ifs) {
        std::cerr << "not exist data file" << std::endl;
        exit(1);
    }
    items.clear();

    //人数, 班数の入力
    int n, g;
    ifs >> n;
    Item::N = n;
    items.reserve(Item::N);
    ifs >> g;
    Group::N = g;

    //Itemの入力
    for (int i = 0; i < Item::N; i++) {
        Item item;
        ifs >> item.gender >> item.year >> item.campus >> item.score;
        item.id = i;
        item.is_leader = false;
        item.relations.resize(Item::N);
        item.times.resize(Item::N);
        item.score_distances.resize(Item::N);
        items.push_back(item);
    }

    //班長の入力
    for (int i = 0; i < Group::N; i++) {
        int id;
        ifs >> id;
        items[id].is_leader = true;
    }

    //関係値の入力
    for (auto&& item1 : items) {
        for (const auto& item2 : items) {
            int r;
            ifs >> r;
            if ((r < 2 || r > 8) && item1.id != item2.id) {
                r = -100;
            }
            item1.relations[item2.id] = r;
        }
    }

    //同じ班になった回数の入力
    for (auto&& item1 : items) {
        for (const auto& item2 : items) {
            ifs >> item1.times[item2.id];
            item1.times[item2.id] *= 4;
        }
    }

    for (auto&& item1 : items) {
        for (const auto& item2 : items) {
            item1.score_distances[item2.id] = std::abs(item1.score - item2.score);
        }
    }
}