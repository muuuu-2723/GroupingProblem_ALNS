#ifndef INPUT_H_
#define INPUT_H_

#include <filesystem>

class Input {
private:
    
public:
    Input(const std::filesystem::path& datafile);
    static std::filesystem::path get_exe_path();
};

#endif