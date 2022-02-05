#ifndef INPUT_H_
#define INPUT_H_

#include <filesystem>
#include <vector>

struct Item;

class Input {
public:
    enum class Opt {
        MIN,
        MAX
    };
private:
    Opt opt;
    std::vector<double> item_relation_params;
    std::vector<double> group_relation_params;
    std::vector<double> value_ave_params;
    std::vector<std::vector<double>> weight_upper;
    std::vector<std::vector<double>> weight_lower;
    std::vector<double> group_cost;
    double constant;
    std::vector<Item> items;
    size_t item_penalty_num;
    size_t group_penalty_num;
    std::filesystem::path exe_directory;
    void read_problem_file(const std::filesystem::path& problem_file_path);
public:
    Input(const std::filesystem::path& problem_file_path, const std::filesystem::path& exe_path);
    const Opt& get_opt() const;
    const std::vector<double>& get_value_ave_params() const;
    const std::vector<double>& get_weight_upper(size_t g_idx) const;
    const std::vector<double>& get_weight_lower(size_t g_idx) const;
    const std::vector<double>& get_group_cost() const;
    double get_constant() const;
    const std::vector<Item>& get_items() const;
    size_t get_item_penalty_num() const;
    size_t get_group_penalty_num() const;
    size_t get_item_relation_params_size() const;
    size_t get_group_relation_params_size() const;
    const std::filesystem::path& get_exe_directory() const;
};

inline const Input::Opt& Input::get_opt() const {
    return opt;
}

inline const std::vector<double>& Input::get_value_ave_params() const {
    return value_ave_params;
}

inline const std::vector<double>& Input::get_weight_upper(size_t g_idx) const {
    return weight_upper[g_idx];
}

inline const std::vector<double>& Input::get_weight_lower(size_t g_idx) const {
    return weight_lower[g_idx];
}

inline const std::vector<double>& Input::get_group_cost() const {
    return group_cost;
}

inline double Input::get_constant() const {
    return constant;
}

inline const std::vector<Item>& Input::get_items() const {
    return items;
}

inline size_t Input::get_item_penalty_num() const {
    return item_penalty_num;
}

inline size_t Input::get_group_penalty_num() const {
    return group_penalty_num;
}

inline size_t Input::get_item_relation_params_size() const {
    return item_relation_params.size();
}

inline size_t Input::get_group_relation_params_size() const {
    return group_relation_params.size();
}

inline const std::filesystem::path& Input::get_exe_directory() const {
    return exe_directory;
}

#endif