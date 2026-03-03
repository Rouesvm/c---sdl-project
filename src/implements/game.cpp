#include <thread>

#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_events.h"

#include "management/AssetManager.hpp"

#include "math/Math.hpp"
#include "math/Vectors.hpp"

#include "renderer/Renderer.hpp"

#include "Game.hpp"

InputState Game::INPUT_STATE;
AssetManager Game::ASSET_MANAGER;

Game::Game(): window_renderer("game"), world() {
    Game::ASSET_MANAGER = AssetManager(window_renderer.currentRenderer());

    ASSET_MANAGER.insertTexture("pickaxe", "asset/player.png");
    ASSET_MANAGER.insertTexture("pickaxe_clicked", "asset/player-clicked.png");
    ASSET_MANAGER.insertTexture("dirt", "asset/dirt.png");

    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < 32; y++) {
            Vector2i position{x, y};
            world.addTile(position, {1});
        }
    }
}

void Game::render() {
    window_renderer.clear();

    world.render(window_renderer.currentRenderer());

    const Texture* texture;
    if (INPUT_STATE.isMouseDown()) {
        texture = ASSET_MANAGER.getTexture("pickaxe_clicked");
    } else texture = ASSET_MANAGER.getTexture("pickaxe");
    window_renderer.currentRenderer().renderTexture(texture, position, PLAYER_SIZE * 8);

    window_renderer.present();
}

void Game::update(double deltaTime) {
    window_renderer.update();
    position = INPUT_STATE.mousePosition() - HALF_SIZE * Game::TILE_SIZE;

    const Vector2f tilePosition = Math::floorDiv(INPUT_STATE.mousePosition(), Game::TILE_SIZE * window_renderer.currentRenderer().zoom);

    if (INPUT_STATE.isMouseDown()) {
        Vector2i position{
            static_cast<int>(tilePosition.x), 
            static_cast<int>(tilePosition.y)
        };
        world.removeTile(position);
    }
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