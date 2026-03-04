#include <string>

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"

#include "renderer/WindowRenderer.hpp"

WindowRenderer::WindowRenderer(const std::string& title) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "didn't work, SDL did not init" << '\n';
        return;
    }

    this->window = SDL_CreateWindow(title.c_str(), 640, 320, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cout << "didn't work, no window created" << '\n';
        return;
    }

    renderer = Renderer(window);
    text_renderer = TextRenderer(renderer);
    renderer.text_renderer = &text_renderer;
}

void WindowRenderer::clear() {
    (void) SDL_RenderClear(renderer.getRenderer());
}

void WindowRenderer::present() {
    currentRenderer().text_renderer->render();
    (void) SDL_RenderPresent(renderer.getRenderer());
}

void WindowRenderer::update() {
    SDL_GetWindowSize(window, &window_size.x, &window_size.y);
}