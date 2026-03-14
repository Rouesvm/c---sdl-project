#pragma once

#include <string>

#include <SDL3/SDL_rect.h>

#include "math/Vectors.hpp"

struct RenderContext {
    SDL_FRect src;
    SDL_FRect dst;
    bool flipped = false;
    int rotation = 0;
};

struct TextContext {
    std::string text;
    Vector2i position;
};

struct SquareContext {
    bool filled = false;
    Vector2i size{};
    Vector2i position{};
    int outline_size = 1;
};