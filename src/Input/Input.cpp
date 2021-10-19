#include "Input.hpp"
#include <Item.hpp>
#include <Group.hpp>
#include <fstream>
#include <filesystem>
#include <Windows.h>
#include <iostream>
#include <string>

using std::vector;

Input::Input(const std::filesystem::path& problem_file, const std::filesystem::path& data_file) {
    std::filesystem::path problem_file_path = problem_file;
    if (!std::filesystem::exists(problem_file)) {
        auto dir = get_exe_path().parent_path();
        while (!std::filesystem::exists(dir / "Data")) {
            if (dir.root_path() == dir) {
                throw std::filesystem::filesystem_error(
                    "Data directory doesn't found",
                    std::make_error_code(std::errc::no_such_file_or_directory)
                );
            }
            dir = dir.parent_path();
        }
        dir.append("Data");
        problem_file_path = dir.append(problem_file.filename().string());
    }

    if (!std::filesystem::exists(problem_file_path)) {
        throw std::filesystem::filesystem_error(
            "problem file doesn't found",
            problem_file_path,
            std::make_error_code(std::errc::no_such_file_or_directory)
        );
    }

    std::filesystem::path data_file_path = data_file;
    if (!std::filesystem::exists(data_file)) {
        auto dir = get_exe_path().parent_path();
        while (!std::filesystem::exists(dir / "Data")) {
            if (dir.root_path() == dir) {
                throw std::filesystem::filesystem_error(
                    "Data directory doesn't found",
                    std::make_error_code(std::errc::no_such_file_or_directory)
                );
            }
            dir = dir.parent_path();
        }
        dir.append("Data");
        data_file_path = dir.append(data_file.filename().string());
    }
    
    if (!std::filesystem::exists(data_file_path)) {
        throw std::filesystem::filesystem_error(
            "data file doesn't found",
            data_file_path,
            std::make_error_code(std::errc::no_such_file_or_directory)
        );
    }

    read_problem_file(problem_file_path, data_file_path);
}

void Input::read_problem_file(const std::filesystem::path& problem_file_path, const std::filesystem::path& data_file_path) {
    std::ifstream ifs;
    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs.open(problem_file_path);

    ifs >> Item::N;
    int group_num;
    ifs >> group_num;
    if (group_num <= 0) {
        Group::N = Item::N;
    }
    else {
        Group::N = group_num;
    }
    ifs >> Item::item_r_size;
    ifs >> Item::group_r_size;
    ifs >> Item::v_size;
    ifs >> Item::w_size;

    read_data_file(data_file_path);

    std::string opt_str;
    ifs >> opt_str;
    if (opt_str == "minimize") {
        opt = Opt::MIN;
    }
    else if (opt_str == "maximize") {
        opt = Opt::MAX;
    }
    else {
        ifs.close();
        throw std::runtime_error("minimize maximize error");
    }

    auto input_params = [](std::ifstream& ifs, vector<double>& params) {
        for (auto&& param : params) {
            ifs >> param;
        }
    };

    item_relation_params.resize(Item::item_r_size);
    input_params(ifs, item_relation_params);

    group_relation_params.resize(Item::group_r_size);
    input_params(ifs, group_relation_params);

    value_ave_params.resize(Item::v_size);
    input_params(ifs, value_ave_params);

    value_sum_params.resize(Item::v_size);
    input_params(ifs, value_sum_params);

    weight_upper.resize(Item::w_size);
    input_params(ifs, weight_upper);

    weight_lower.resize(Item::w_size);
    input_params(ifs, weight_lower);

    ifs >> item_penalty_num;
    for (size_t i = 0; i < item_penalty_num; ++i) {
        int id1, id2;
        ifs >> id1 >> id2;
        items[id1].item_penalty[id2] = 1;
        items[id2].item_penalty[id1] = 1;
    }

    ifs >> group_penalty_num;
    for (size_t i = 0; i < group_penalty_num; ++i) {
        int item_id, group_id;
        ifs >> item_id >> group_id;
        items[item_id].group_penalty[group_id] = 1;
    }

    size_t num;
    ifs >> num;
    for (size_t i = 0; i < num; ++i) {
        int item_id, group_id;
        ifs >> item_id >> group_id;
        items[item_id].predefined_group = group_id;
    }

    ifs.close();
}

void Input::read_data_file(const std::filesystem::path& file_path) {
    std::ifstream ifs;
    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs.open(file_path);

    items.reserve(Item::N);
    for (size_t i = 0; i < Item::N; ++i) {
        Item item;
        item.id = i;
        item.predefined_group = -1;
        item.weight.resize(Item::w_size);
        for (auto&& w : item.weight) {
            ifs >> w;
        }

        item.values.resize(Item::v_size);
        for (auto&& v : item.values) {
            ifs >> v;
        }

        item.item_relations.assign(Item::N, vector<double>(Item::item_r_size, 0));
        item.group_relations.assign(Group::N, vector<double>(Item::group_r_size, 0));
        item.item_penalty.assign(Item::N, 0);
        item.group_penalty.assign(Group::N, 0);

        items.push_back(item);
    }

    for (size_t i = 0; i < Item::item_r_size; ++i) {
        for (auto&& item : items) {
            for (auto&& item_r : item.item_relations) {
                ifs >> item_r[i];
            }
        }
    }

    for (size_t i = 0; i < Item::group_r_size; ++i) {
        for (auto&& item : items) {
            for (auto&& group_r : item.group_relations) {
                ifs >> group_r[i];
            }
        }
    }

    ifs.close();
}

std::filesystem::path Input::get_exe_path() {
    wchar_t path[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, path, MAX_PATH);
    return path;
}