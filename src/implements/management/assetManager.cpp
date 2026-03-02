#include "management/AssetManager.hpp"
#include "SDL3/SDL_render.h"
#include "renderer/Renderer.hpp"

AssetManager::AssetManager(Renderer& renderer): renderer(&renderer) {

}

void AssetManager::insertTexture(const std::string& name, const std::string& filepath) {
    if (string_to_textures.find(name) != string_to_textures.end()) return;

    Texture texture = renderer->loadTexture(filepath);
    textures.push_back(std::move(texture));

    Texture& ptr = textures.back();
    string_to_textures.try_emplace(name, &ptr);

    SDL_SetTextureScaleMode(texture.texture, SDL_ScaleMode::SDL_SCALEMODE_PIXELART);
}

Texture* AssetManager::getTexture(const std::string& name) {
    return string_to_textures.at(name);
}