#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"

#include "Game.hpp"

#include "math/Vectors.hpp"
#include "renderer/Context.hpp"

#include "renderer/Renderer.hpp"
#include "renderer/TextRenderer.hpp"

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
    (void) SDL_RenderTexture(renderer, texture->texture, &context.src, &context.dst);
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
    SDL_FRect rect{};
    (void) SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
    for (const SquareContext& context : squares) {
        rect.w = context.scale * Game::TILE_SIZE_IN_PIXELS;
        rect.h = context.scale * Game::TILE_SIZE_IN_PIXELS;
        rect.x = context.position.x;
        rect.y = context.position.y;
        !context.filled ? (void) SDL_RenderRect(renderer, &rect) : (void) SDL_RenderFillRect(renderer, &rect);
    }
    (void) SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    squares.clear();
}