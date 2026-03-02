#pragma once

#include <unordered_map>
#include <vector>

#include <SDL3/SDL_render.h>

#include <renderer/Renderer.hpp>

class AssetManager {
    private:
        Renderer* renderer = nullptr;
    private:
        std::vector<Texture> textures{};
        std::unordered_map<std::string, Texture*> string_to_textures{};
    public:
        AssetManager() {};
        AssetManager(Renderer&);
    public:
        void insertTexture(const std::string&, const std::string&);
        Texture* getTexture(const std::string&);
};