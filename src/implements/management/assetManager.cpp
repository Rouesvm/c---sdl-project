#include "management/AssetManager.hpp"
#include "SDL3/SDL_render.h"
#include "renderer/Renderer.hpp"

AssetManager::AssetManager(Renderer& renderer): renderer(&renderer) {

}

void AssetManager::insertTexture(const std::string& name, const std::string& filepath) {
    if (string_to_textures.find(name) != string_to_textures.end()) return;

    Texture texture = renderer->loadTexture(filepath);
    textures.push_back(std::move(texture));

    size_t index = textures.size() - 1;
    SDL_SetTextureScaleMode(
        textures[index].texture, 
        SDL_ScaleMode::SDL_SCALEMODE_PIXELART
    );

    string_to_textures.emplace(name, index);
}

Texture* AssetManager::getTexture(const std::string& name) {
    return &textures[string_to_textures.at(name)];
}