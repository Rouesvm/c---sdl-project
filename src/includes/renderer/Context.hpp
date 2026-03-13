#pragma once

#include <string>

#include <SDL3/SDL_rect.h>

#include "math/Vectors.hpp"

struct RenderContext {
    SDL_FRect src;
    SDL_FRect dst;
};

struct TextContext {
    std::string text;
    Vector2i position;
};

struct SquareContext {
    bool filled = false;
    float scale = 1;
    Vector2i position{};
};