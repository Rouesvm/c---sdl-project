#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

#include "world/objects/Tile.hpp"

struct RecipeInput {
    uint8_t item_id = 0;
};

class Recipe {
    private:
        std::vector<RecipeInput> inputs{};
        Resource output{};
        std::string base_hash{};
    public:
        Recipe& setOutput(const RecipeInput& newOutput) {
            this->output = Resource{newOutput.item_id, 1};
            return *this;
        }
        Recipe& add(const RecipeInput& input) {
            this->inputs.push_back(input);
            this->base_hash += std::to_string(input.item_id);
            return *this;
        };
    public:
        const std::string& getRecipe() {
            return this->base_hash;
        }
        const Resource& getOutput() {
            return output;
        }
};

class RecipeManager {
    private:
        std::unordered_map<std::string, Resource> recipes{};
        std::vector<std::string> recipe_hashs{};
    public:
        void addRecipe(Recipe& recipe);
        Resource& getRecipeOutput(Recipe& resources);
};