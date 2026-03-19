#include <vector>

#include "world/objects/Tile.hpp"
#include "management/RecipeManager.hpp"

void RecipeManager::addRecipe(Recipe& recipe) {
    if (recipes.find(recipe.getRecipe()) != recipes.end()) return;
    recipes.try_emplace(recipe.getRecipe(), recipe.getOutput());
    recipe_hashs.push_back(recipe.getRecipe());
}  

Resource& RecipeManager::getRecipeOutput(Recipe& recipe) {   
    auto output = recipes.find(recipe.getRecipe());
    if (output != recipes.end()) 
        return output->second;   
    else return Resource::empty();
}

