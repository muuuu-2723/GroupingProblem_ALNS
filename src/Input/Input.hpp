#ifndef INPUT_H_
#define INPUT_H_

#include <fstream>
#include <filesystem>

class Input {
private:
    
public:
    Input();
    static std::filesystem::path get_exe_path();
};

#endif