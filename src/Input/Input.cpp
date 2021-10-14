#include "Input.hpp"
#include <fstream>
#include <filesystem>
#include <Windows.h>
#include <iostream>

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

std::filesystem::path Input::get_exe_path() {
    wchar_t path[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, path, MAX_PATH);
    return path;
}