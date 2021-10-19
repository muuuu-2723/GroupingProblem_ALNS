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
    std::vector<double> value_sum_params;
    std::vector<double> weight_upper;
    std::vector<double> weight_lower;
    std::vector<Item> items;
    size_t item_penalty_num;
    size_t group_penalty_num;
    void read_problem_file(const std::filesystem::path& problem_file_path, const std::filesystem::path& data_file_path);
    void read_data_file(const std::filesystem::path& file_path);
public:
    static std::filesystem::path get_exe_path();
    Input(const std::filesystem::path& problem_file_path, const std::filesystem::path& data_file_path);
    const std::vector<double>& get_item_relation_params() const;
    const std::vector<double>& get_group_relation_params() const;
    const std::vector<double>& get_value_ave_params() const;
    const std::vector<double>& get_value_sum_params() const;
    const std::vector<double>& get_weight_upper() const;
    const std::vector<double>& get_weight_lower() const;
    const std::vector<Item>& get_items() const;
    size_t get_item_penalty_num() const;
    size_t get_group_penalty_num() const;
};

inline const std::vector<double>& Input::get_item_relation_params() const {
    return item_relation_params;
}

inline const std::vector<double>& Input::get_group_relation_params() const {
    return group_relation_params;
}

inline const std::vector<double>& Input::get_value_ave_params() const {
    return value_ave_params;
}

inline const std::vector<double>& Input::get_value_sum_params() const {
    return value_sum_params;
}

inline const std::vector<double>& Input::get_weight_upper() const {
    return weight_upper;
}

inline const std::vector<double>& Input::get_weight_lower() const {
    return weight_lower;
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

#endif