#pragma once

#include <vector>
#include <unordered_map>

#include <SDL3/SDL_render.h>

#include <renderer/Renderer.hpp>

class AssetManager {
    private:
        Renderer* renderer = nullptr;
    private:
        std::unordered_map<std::string, int> string_to_textures{};
        std::vector<Texture> textures{};
    public:
        AssetManager() {};
        AssetManager(Renderer&);
        ~AssetManager() {
            for (Texture& texture : textures) {
                SDL_DestroyTexture(texture.texture);
            }
        }
    public:
        void insertTexture(const std::string&, const std::string&);
        const Texture* getTexture(const std::string&) const;
        const Texture* getTexture(int) const;
        int storedTextureSize() const {return textures.size();};
};      