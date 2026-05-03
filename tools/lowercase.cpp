#include <iostream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

std::string to_lower(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

int main() {
    try {
        for (const auto& entry : fs::directory_iterator(fs::current_path())) {
            if (!entry.is_regular_file()) continue;

            fs::path old_path = entry.path();
            std::string name = old_path.filename().string();
            std::string lower = to_lower(name);

            if (name != lower) {
                fs::path new_path = old_path.parent_path() / lower;

                if (!fs::exists(new_path)) {
                    std::cout << "[RENAME] " << name << " -> " << lower << std::endl;
                    fs::rename(old_path, new_path);
                } else {
                    fs::path temp_path = old_path.parent_path() / ("__tmp__" + name);
                    std::cout << "[RENAME] " << name << " -> (temp) -> " << lower << std::endl;

                    fs::rename(old_path, temp_path);
                    fs::rename(temp_path, new_path);
                }
            } else {
                std::cout << "[SKIP] " << name << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }

    std::cout << "done" << std::endl;
    std::cin.get();
    return 0;
}