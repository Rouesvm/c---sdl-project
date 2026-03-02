#include "Game.hpp"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_rect.h"
#include "management/AssetManager.hpp"
#include "math/Vectors.hpp"

#include <thread>

Game::Game(): window_renderer("game"), asset_manager() {
    this->asset_manager = AssetManager(window_renderer.currentRenderer());

    asset_manager.insertTexture("box", "asset/player.png");
}

void Game::render() {
    window_renderer.clear();
    Vector2f position{320, 160}, size{80, 80};
    window_renderer.currentRenderer().renderTexture(asset_manager.getTexture("box"), position, size);
    window_renderer.present();
}

void Game::poll() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        
        bool keyUp = false;
        bool keyDown = false;

        (void) keyUp;

        switch (event.type) {
            case SDL_EVENT_QUIT: running = false; break;
            case SDL_EVENT_KEY_UP: keyUp = true; break;
            case SDL_EVENT_KEY_DOWN: keyDown = true; break;
        }
    }
}

void Game::update() {

}

void Game::tick() {
    poll();
    update();
}

void preciseSleep(const double seconds) {
    if (seconds <= 0.0
    ) return;

    const uint64_t frequency = SDL_GetPerformanceFrequency();
    const uint64_t targetTicks =
        SDL_GetPerformanceCounter() +
        static_cast<uint64_t>(seconds * frequency);

    while (true) {
        const uint64_t now = SDL_GetPerformanceCounter();
        const int64_t remaining = static_cast<int64_t>(targetTicks - now);

        if (remaining < 0) break;

        const double remainingInMS = (static_cast<double>(remaining) / frequency) * 1000.0;

        if (remainingInMS > 2.0) {
            SDL_Delay(static_cast<uint32_t>(remainingInMS - 1.0));
        } else std::this_thread::yield();
    }
}

void Game::loop() {
    const double frequency = static_cast<double>(SDL_GetPerformanceFrequency());
    uint64_t lastCounter = SDL_GetPerformanceCounter();

    double accumulator = 0.0;

    uint64_t fpsTimer = SDL_GetPerformanceCounter();

    double PHYSICS_STEP = 1 / 60.0;
    double FRAME_DURATION = 1 / 60.0;

    while (running) {
        const uint64_t frameStart = SDL_GetPerformanceCounter();

        const double deltaTime = static_cast<double>(frameStart - lastCounter) / frequency;
        lastCounter = frameStart;
        accumulator += deltaTime;

        const double MAX_ACCUMULATOR = PHYSICS_STEP * 5.0;
        accumulator = std::min(accumulator, MAX_ACCUMULATOR);

        while (accumulator >= PHYSICS_STEP) {
            this->tick();
            accumulator -= PHYSICS_STEP;
        }

        double alpha = accumulator / PHYSICS_STEP;
        this->render();

        const double finalTime = static_cast<double>(SDL_GetPerformanceCounter() - frameStart) / frequency;
        if (FRAME_DURATION > 0 && finalTime < FRAME_DURATION) {
            preciseSleep(FRAME_DURATION - finalTime);
        }  
    }

    (void) SDL_Quit();

}