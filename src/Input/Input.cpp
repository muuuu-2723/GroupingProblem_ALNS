#include "Input.hpp"
#include <filesystem>
#include <Windows.h>

std::filesystem::path Input::get_exe_path() {
    wchar_t path[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, path, MAX_PATH);
    return path;
}