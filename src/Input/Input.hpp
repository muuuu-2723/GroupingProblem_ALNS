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
    void read_problem_file(const std::filesystem::path& file_path);
    void read_data_file(const std::filesystem::path& file_path);
public:
    Input(const std::filesystem::path& datafile);
    static std::filesystem::path get_exe_path();
};

#endif