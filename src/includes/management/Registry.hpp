
#include <string>
#include <vector>
#include <unordered_map>

class Registry {
    std::unordered_map<std::string, int> block_to_id{};
    std::vector<std::string> id_to_block{};
};