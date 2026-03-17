#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <vector>

#include "Context.hpp"
#include "math/Vectors.hpp"

struct Texture {
    SDL_Texture* texture;
};

class TextRenderer;
class Renderer {
    private:
        std::vector<SDL_FRect> rectangles{};
    private:
        SDL_Renderer* renderer = nullptr;
    public:
        TextRenderer* text_renderer = nullptr;
    public:
        float zoom = 2.0f;
    public:
        Renderer() {};
        Renderer(SDL_Window*);
    public:
        Texture loadTexture(const std::string&);

        void renderTexture(const Texture*, const RenderContext&);
        void renderTexture(const Texture*, const Vector2f& position, const Vector2f& size);
        void renderText(TextContext&);
        void renderSquare(SquareContext&);

        void render();

        SDL_Renderer* getRenderer();
};