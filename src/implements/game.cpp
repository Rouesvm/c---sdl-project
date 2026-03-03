#include <thread>

#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_events.h"

#include "management/AssetManager.hpp"

#include "math/Math.hpp"
#include "math/Vectors.hpp"

#include "renderer/Renderer.hpp"

#include "Game.hpp"

InputState Game::INPUT_STATE;

Game::Game(): window_renderer("game"), asset_manager() {
    this->asset_manager = AssetManager(window_renderer.currentRenderer());

    asset_manager.insertTexture("pickaxe", "asset/player.png");
    asset_manager.insertTexture("pickaxe_clicked", "asset/player-clicked.png");
    asset_manager.insertTexture("block", "asset/block.png");
}

void Game::render() {
    window_renderer.clear();

    Texture* texture;
    if (inputState().isMouseDown()) {
        texture = asset_manager.getTexture("pickaxe_clicked");
    } else texture = asset_manager.getTexture("pickaxe");
    window_renderer.currentRenderer().renderTexture(texture, position * 8, PLAYER_SIZE * 8);

    window_renderer.present();
}

void Game::update(double deltaTime) {
    window_renderer.update();

    Vector2f velocity{};
    Vector2f finalPosition{position};

    velocity.y = 4.5 * deltaTime;
    finalPosition.y += velocity.y;

    Vector2i actualSize{window_renderer.windowSize() / 8};
    if (actualSize.x > (position.x + PLAYER_SIZE.x) && actualSize.y > (position.y + PLAYER_SIZE.y)) {
        position = finalPosition;
    } else position -= (finalPosition - position);
}

void Game::poll() {
    Vector2f mousePosition;
    (void) SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
    INPUT_STATE.setMousePosition(mousePosition);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        bool keyUp = event.type == SDL_EVENT_KEY_UP && event.key.repeat == 0;
        bool keyDown = event.type == SDL_EVENT_KEY_DOWN && event.key.repeat == 0;

        INPUT_STATE.setMouseDown(event.type == SDL_EventType::SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT);

        switch (event.type) {
            case SDL_EVENT_QUIT: running = false; break;
        }
    }
}

void Game::tick(double deltaTime) {
    poll();
    update(deltaTime);
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

    while (running) {
        const uint64_t frameStart = SDL_GetPerformanceCounter();

        const double deltaTime = static_cast<double>(frameStart - lastCounter) / frequency;
        lastCounter = frameStart;
        accumulator += deltaTime;

        const double MAX_ACCUMULATOR = PHYSICS_STEP * 5.0;
        accumulator = std::min(accumulator, MAX_ACCUMULATOR);

        while (accumulator >= PHYSICS_STEP) {
            this->tick(PHYSICS_STEP);
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