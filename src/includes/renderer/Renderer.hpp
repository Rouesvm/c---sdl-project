#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

#include "Context.hpp"
#include "math/Vectors.hpp"

struct Texture {
    SDL_Texture* texture;
    Vector2i size;
};

class Renderer {
    private:
        SDL_Renderer* renderer;
    public:
        Renderer() {};
        Renderer(SDL_Window*);
    public:
        Texture loadTexture(const std::string&);
        void renderTexture(Texture*, RenderContext&);
        void renderTexture(Texture*, Vector2f& position, Vector2f& size);
        SDL_Renderer* getRenderer();
};