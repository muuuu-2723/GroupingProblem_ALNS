#ifndef DEBUG_H_
#define DEBUG_H_

#include <matplotlib.hpp>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <memory>

class Solution;
class DiscreteDistribution;
class Input;

class Debug {
private:
    matplotlib g;
    const DiscreteDistribution& search_random;
    const DiscreteDistribution& destroy_random;
    const std::unique_ptr<Solution>& solution;
    const Solution& best;
    const int& x;
    int debug_num;
    std::ofstream eval_out;
    std::ofstream search_out;
    std::ofstream destroy_out;
    std::string fig_file_path;
    std::filesystem::path item_times_path;
    std::filesystem::path group_times_path;
    std::vector<std::string> color_map;
    std::vector<double> prev_p;
    double prev_eval;
    double prev_best_eval;
    double lower_eval;
    int param;
    
public:
    Debug(const DiscreteDistribution& search_random, const DiscreteDistribution& destroy_random, const std::unique_ptr<Solution>& solution,
          const Solution& best, const int& x, const std::string& problem_file, int debug_num, int max_x, double max_eval, const Input& input);
    ~Debug();
    void output();
};

#endif