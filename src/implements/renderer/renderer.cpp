#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"

#include "Game.hpp"

#include "math/Vectors.hpp"
#include "renderer/Context.hpp"

#include "renderer/Renderer.hpp"
#include "renderer/TextRenderer.hpp"
#include <algorithm>
#include <vector>

Renderer::Renderer(SDL_Window* window) {
    this->renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cout << "didn't work, renderer didn't want to exist" << '\n';
        return;
    }
}

SDL_Renderer* Renderer::getRenderer() {
    return renderer;
}

Texture Renderer::loadTexture(const std::string& filename) {
    SDL_Surface* surface = SDL_LoadPNG(filename.c_str());
    if (surface == nullptr) {
        std::cout << "Failed to load texture: " + filename << '\n';
        return {};
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr) {
        std::cout << "Failed to create texture from surface: " + filename << '\n';
        return {};
    }

    return Texture{
        texture
    };
}

void Renderer::renderTexture(const Texture* texture, RenderContext& context) {
    if (context.rotation == 0) 
        (void) SDL_RenderTexture(renderer, texture->texture, &context.src, &context.dst);
    else {
        (void) SDL_RenderTextureRotated(renderer, texture->texture, &context.src, &context.dst, context.rotation, {}, context.flipped ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    }
} 

void Renderer::renderTexture(const Texture* texture, const Vector2f& position, const Vector2f& size) {
    SDL_FRect rect{position.x, position.y, size.x, size.y};
    (void) SDL_RenderTexture(renderer, texture->texture, nullptr, &rect);
} 

void Renderer::renderText(TextContext& context) {
    text_renderer->addText(context);
}

void Renderer::renderSquare(SquareContext& context) {
    squares.push_back(context);
}

void Renderer::render() {
    int reserveSize = 0;
    for (const SquareContext& context : squares) {
        if (context.filled) reserveSize++;
        else reserveSize += 4;
    }

    rectangles.clear();
    rectangles.reserve(reserveSize);

    if (!squares.empty()) {
        SDL_FRect rect{};

        for (const SquareContext& context : squares) {
            rect.w = context.size.x;
            rect.h = context.size.y;
            rect.x = context.position.x;
            rect.y = context.position.y;
        
            if (context.filled) {
                rectangles.push_back(rect);
            } else {
                float t = std::min(static_cast<float>(context.outline_size), rect.w * 0.5f);
                rectangles.push_back({ rect.x, rect.y, rect.w, t });
                rectangles.push_back({ rect.x, rect.y + rect.h - t, rect.w, t });
                rectangles.push_back({ rect.x, rect.y, t, rect.h });
                rectangles.push_back({ rect.x + rect.w - t, rect.y, t, rect.h });
            }
        }
        
        (void) SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
        SDL_RenderFillRects(renderer, rectangles.data(), rectangles.size());
        (void) SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    }

    squares.clear();
}