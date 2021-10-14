#include "Input.hpp"
#include <Item.hpp>
#include <fstream>
#include <filesystem>
#include <Windows.h>
#include <iostream>
#include <string>

using std::vector;

Input::Input(const std::filesystem::path& datafile) {
    std::filesystem::path datafile_path = datafile;
    if (!std::filesystem::exists(datafile)) {
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
        datafile_path = dir.append(datafile.filename().string());
    }
    
    if (!std::filesystem::exists(datafile_path)) {
        throw std::filesystem::filesystem_error(
            "data file doesn't found",
            datafile_path,
            std::make_error_code(std::errc::no_such_file_or_directory)
        );
    }
}

void Input::read_problem_file(const std::filesystem::path& file_path) {
    std::ifstream ifs;
    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs.open(file_path);
    std::string opt_str;
    ifs >> opt_str;
    if (opt_str == "minimize") {
        opt = Opt::MIN;
    }
    else if (opt_str == "maximize") {
        opt = Opt::MAX;
    }
    else {
        throw std::runtime_error("minimize maximize error");
    }

    auto input_params = [](std::ifstream& ifs, vector<double>& params) {
        for (auto&& param : params) {
            ifs >> param;
        }
    };

    ifs >> Item::item_r_size;
    item_relation_params.resize(Item::item_r_size);
    input_params(ifs, item_relation_params);

    ifs >> Item::group_r_size;
    group_relation_params.resize(Item::group_r_size);
    input_params(ifs, group_relation_params);

    ifs >> Item::v_size;
    value_ave_params.resize(Item::v_size);
    input_params(ifs, value_ave_params);

    value_sum_params.resize(Item::v_size);
    input_params(ifs, value_sum_params);

    ifs >> Item::w_size;
    weight_upper.resize(Item::w_size);
    input_params(ifs, weight_upper);

    weight_lower.resize(Item::w_size);
    input_params(ifs, weight_lower);

    ifs.close();
}

void Input::read_data_file(const std::filesystem::path& file_path) {
    std::ifstream ifs;
    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs.open(file_path);

    ifs.close();
}

std::filesystem::path Input::get_exe_path() {
    wchar_t path[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, path, MAX_PATH);
    return path;
}