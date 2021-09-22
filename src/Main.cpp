#include <Person.hpp>
#include <Group.hpp>
#include <Solution.hpp>
#include <MyRandom.hpp>
#include <Search.hpp>
#include <Destroy.hpp>
#include <matplotlib.hpp>

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

vector<Person> persons;
std::filesystem::path get_exe_path();
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
        is_debug = true;
    }
    matplotlib g;

    auto data_dir = get_exe_path().parent_path().parent_path().append("Data");
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
        if (is_debug) {
            output_dir = get_exe_path().parent_path();
            eval_out.open(output_dir / ("eval_" + datafile.string()));
            search_out.open(output_dir / ("search_" + datafile.string()));
            destroy_out.open(output_dir / ("destroy_" + datafile.string()));
            if (debug_num != 0) g.open();
            if (debug_num == 1) {
                g.screen(0, 45000, M, 46000);
            }
            else if (debug_num != 0) {
                color_map = {"turquoise", "orange", "green", "tomato", "orchid", "sienna", "fuchsia", "grey", "gold", "cyan"};
                g.screen(0, 0, M, 100);
            }
        }

        auto start = std::chrono::high_resolution_clock::now();
        Solution now(persons);
        now.evaluation_all(persons);
        std::cerr << now;
        Solution best(now);
        /*auto t_begin = now.debug_evaluation_all(persons);
        std::cout << std::get<2>(t_begin) << std::endl;*/

        vector<std::unique_ptr<Search>> searches;
        searches.emplace_back(std::make_unique<PenaltyGreedy>(persons, 1));
        searches.emplace_back(std::make_unique<RelationGreedy>(persons, 1));
        searches.emplace_back(std::make_unique<ShiftNeighborhood>(persons, 1));
        searches.emplace_back(std::make_unique<SwapNeighborhood>(persons, /*4*/2));
        searches.emplace_back(std::make_unique<NeighborhoodGraph>(persons, 4));
        searches.emplace_back(std::make_unique<ScoreGreedy>(persons, 1));

        std::shared_ptr<RandomDestroy> random_destroy = std::make_shared<RandomDestroy>(persons, (1.5 * Person::N) / Group::N, 1);
        std::shared_ptr<MinimumDestroy> minimum_destroy = std::make_shared<MinimumDestroy>(persons, (1.5 * Person::N) / Group::N, 1);

        vector<std::shared_ptr<Destroy>> destructions;
        destructions.emplace_back(random_destroy);
        destructions.emplace_back(std::make_shared<RandomGroupDestroy>(persons, Group::N / 3, 1));
        destructions.emplace_back(minimum_destroy);
        destructions.emplace_back(std::make_shared<MinimumGroupDestroy>(persons, Group::N / 3, 1));
        /*destructions.emplace_back(std::make_shared<RandomDestroy>(persons, Person::N / Group::N));
        destructions.emplace_back(std::make_shared<MinimumDestroy>(persons, Person::N / Group::N));*/
        destructions.emplace_back(std::make_shared<Destroy>(persons, 1));

        vector<double> search_weights(searches.size(), 1);
        vector<double> destroy_weights(destructions.size() - 1, 1);

        int cnt = 0;
        int best_change_cnt = cnt;
        double prev_next_eval;
        if (is_debug) {
            eval_out << std::fixed << std::setprecision(8) << cnt << " " << now.get_eval_value() << " " << best.get_eval_value() << std::endl;
            if (debug_num != 0) {
                prev_next_eval = now.get_eval_value();
            }
        }
        vector<double> search_time(searches.size(), 0);
        vector<int> counter(searches.size(), 0);
        int moving_idx = -1;
        vector<vector<int>> score_cnt(searches.size(), vector<int>(4, 0));

        while (cnt < M) {
            auto search_itr = searches.begin();
            std::generate(search_weights.begin(), search_weights.end(), [&search_itr]() { return (*search_itr++)->get_weight(); });
            DiscreteDistribution search_random(search_weights);
            auto destroy_itr = destructions.begin();
            std::generate(destroy_weights.begin(), destroy_weights.end(), [&destroy_itr]() { return (*destroy_itr++)->get_weight(); });
            DiscreteDistribution destroy_random(destroy_weights);

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
            if (is_debug) {
                search_p = search_random.get_probabilities();
                destroy_p = destroy_random.get_probabilities();
                
                search_out << cnt;
                for (auto&& p : search_p) {
                    search_out << " " << p * 100;
                }
                search_out << std::endl;
                destroy_out << cnt;
                for (auto&& p : destroy_p) {
                    destroy_out << " " << p * 100;
                }
                destroy_out << std::endl;
            }

            //std::cerr << search_idx << std::endl;
            auto sstart = std::chrono::high_resolution_clock::now();
            Solution next_solution = (*searches[search_idx])(now, destructions[destroy_idx]);
            auto send = std::chrono::high_resolution_clock::now();
            double stime = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(send - sstart).count() / 1000.0);
            search_time[search_idx] += stime;
            ++counter[search_idx];
            double prev_now_eval, prev_best_eval;
            if (is_debug && debug_num != 0) {
                prev_now_eval = now.get_eval_value();
                prev_best_eval = best.get_eval_value();
                //g.line(cnt, prev_next_eval, cnt + 1, next_solution.get_eval_value(), "cyan");
                prev_next_eval = next_solution.get_eval_value();
            }

            if (next_solution.get_eval_value() > best.get_eval_value() - std::abs(best.get_eval_value()) * 0.005 && random_destroy->get_destroy_num() > (0.5 * Person::N) / Group::N) {
                random_destroy->set_destroy_num((0.5 * Person::N) / Group::N);
                minimum_destroy->set_destroy_num((0.5 * Person::N) / Group::N);
            }

            double score;
            if (next_solution.get_eval_value() > best.get_eval_value()) {
                score = 100;
                now = std::move(next_solution);
                best = now;
                best_change_cnt = cnt;
                if (cnt != 0 && random_destroy->get_destroy_num() > (0.5 * Person::N) / Group::N) {
                    random_destroy->set_destroy_num((0.5 * Person::N) / Group::N);
                }
                if (cnt != 0 && minimum_destroy->get_destroy_num() > (0.5 * Person::N) / Group::N) {
                    minimum_destroy->set_destroy_num((0.5 * Person::N) / Group::N);
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

            if (is_debug) {
                eval_out << cnt + 1 << " " << now.get_eval_value() << " " << best.get_eval_value() << std::endl;
                if (debug_num == 1) {
                    g.line(cnt, prev_now_eval, cnt + 1, now.get_eval_value(), "lightgreen");
                    g.line(cnt, prev_best_eval, cnt + 1, best.get_eval_value(), "red");
                }
                else if (debug_num != 0) {
                    g.line(cnt, prev_now_eval - 700, cnt + 1, now.get_eval_value() - 700, "lightgreen");
                    g.line(cnt, prev_best_eval - 700, cnt + 1, best.get_eval_value() - 700, "red");
                }
                /*else*/ if (debug_num == 2) {
                    search_itr = searches.begin();
                    std::generate(search_weights.begin(), search_weights.end(), [&search_itr]() { return (*search_itr++)->get_weight(); });
                    DiscreteDistribution next_search(search_weights);
                    vector<double> next_p = next_search.get_probabilities();
                    for (int j = 0; j < search_p.size(); ++j) {
                        g.line(cnt, search_p[j] * 100, cnt + 1, next_p[j] * 100, color_map[j].c_str());
                    }
                }
                else if (debug_num == 3) {
                    destroy_itr = destructions.begin();
                    std::generate(destroy_weights.begin(), destroy_weights.end(), [&destroy_itr]() { return (*destroy_itr++)->get_weight(); });
                    DiscreteDistribution next_destroy(destroy_weights);
                    vector<double> next_p = next_destroy.get_probabilities();
                    for (int j = 0; j < destroy_weights.size(); ++j) {
                        g.line(cnt, destroy_p[j] * 100, cnt + 1, next_p[j] * 100, color_map[j].c_str());
                    }
                }
            }

            if (cnt % (M / 100) == 0) {
                random_destroy->add_destroy_num(-1);
                minimum_destroy->add_destroy_num(-1);
            }
            if ((cnt - best_change_cnt) % (int)((M / 100) * 0.75 * (Person::N / Group::N)) == 0 && cnt != best_change_cnt) {
                random_destroy->add_destroy_num(Person::N / Group::N);
                minimum_destroy->add_destroy_num(Person::N / Group::N);
            }

            ++cnt;
            //if (cnt > M / 3 && cnt > best_change_cnt * 2) break;
        }
        if (is_debug) {
            DiscreteDistribution next_search(search_weights);
            search_out << cnt;
            for (auto&& p : next_search.get_probabilities()) {
                search_out << " " << p * 100;
            }
            search_out << std::endl;
            DiscreteDistribution next_destroy(destroy_weights);
            destroy_out << cnt;
            for (auto&& p : next_destroy.get_probabilities()) {
                destroy_out << " " << p * 100;
            }
            destroy_out << std::endl;
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
        best.evaluation_all(persons);
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
    persons.clear();

    //人数, 班数の入力
    int n, g;
    ifs >> n;
    Person::N = n;
    persons.reserve(Person::N);
    ifs >> g;
    Group::N = g;

    //Personの入力
    for (int i = 0; i < Person::N; i++) {
        Person p;
        ifs >> p.gender >> p.year >> p.campus >> p.score;
        p.id = i;
        p.is_leader = false;
        p.relations.resize(Person::N);
        p.times.resize(Person::N);
        p.score_distances.resize(Person::N);
        persons.push_back(p);
    }

    //班長の入力
    for (int i = 0; i < Group::N; i++) {
        int id;
        ifs >> id;
        persons[id].is_leader = true;
    }

    //関係値の入力
    for (auto&& p1 : persons) {
        for (const auto& p2 : persons) {
            int r;
            ifs >> r;
            if ((r < 2 || r > 8) && p1.id != p2.id) {
                r = -100;
            }
            p1.relations[p2.id] = r;
        }
    }

    //同じ班になった回数の入力
    for (auto&& p1 : persons) {
        for (const auto& p2 : persons) {
            ifs >> p1.times[p2.id];
            p1.times[p2.id] *= 4;
        }
    }

    for (auto&& p1 : persons) {
        for (const auto& p2 : persons) {
            p1.score_distances[p2.id] = std::abs(p1.score - p2.score);
        }
    }
}

std::filesystem::path get_exe_path() {
    wchar_t path[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, path, MAX_PATH);
    return path;
}