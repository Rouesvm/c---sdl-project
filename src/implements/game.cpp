#include <thread>

#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_events.h"

#include "management/AssetManager.hpp"

#include "math/Math.hpp"
#include "math/Vectors.hpp"

#include "player/Camera.hpp"
#include "renderer/Context.hpp"
#include "renderer/Renderer.hpp"

#include "Game.hpp"

InputState Game::INPUT_STATE;
ClientState Game::CLIENT_STATE;
AssetManager Game::ASSET_MANAGER;

Game::Game(): window_renderer("game"), camera(), world() {
    Game::ASSET_MANAGER = AssetManager(window_renderer.currentRenderer());

    ASSET_MANAGER.insertTexture("pickaxe", "asset/player.png");
    ASSET_MANAGER.insertTexture("pickaxe_clicked", "asset/player-clicked.png");

    int start = ASSET_MANAGER.storedTextureSize();
    ASSET_MANAGER.insertTexture("dirt", "asset/dirt.png");
    ASSET_MANAGER.insertTexture("stone", "asset/furnace-2.png");
    ASSET_MANAGER.insertTexture("furnace", "asset/furnace.png");
    int finalSize = ASSET_MANAGER.storedTextureSize();

    for (int i = 0; i < finalSize - start; i++) {
        world.textures[i] = start + i;
    }
}

void Game::render() {
    window_renderer.clear();

    Renderer& renderer = window_renderer.currentRenderer();
    world.render(renderer);

    const Texture* texture;
    if (INPUT_STATE.isMouseDown()) {
        texture = ASSET_MANAGER.getTexture("pickaxe_clicked");
    } else texture = ASSET_MANAGER.getTexture("pickaxe");

    TextContext context{
        "FPS: 0",
        {0, 0}
    };

    renderer.renderText(context);
    renderer.renderTexture(texture, position, PLAYER_SIZE * 8);

    window_renderer.present();
}

void Game::update(double deltaTime) {
    window_renderer.update();
    position = INPUT_STATE.mousePosition() - HALF_SIZE * Game::TILE_SIZE_IN_PIXELS;

    camera.update(Game::CLIENT_STATE.windowSize(), player_position, deltaTime);

    Game::CLIENT_STATE.setCameraPosition(camera.screen_position);
    Game::CLIENT_STATE.setWindowSize(window_renderer.windowSize());

    const Vector2f tilePosition = Math::floorDiv(
        INPUT_STATE.mousePosition() / window_renderer.currentRenderer().zoom
        + CLIENT_STATE.cameraPosition(), 
        Game::TILE_SIZE_IN_PIXELS
    );

    if (INPUT_STATE.isMouseDown()) {
        Vector2i position{
            static_cast<int>(tilePosition.x), 
            static_cast<int>(tilePosition.y)
        };

        if (!INPUT_STATE.isLeft()) {
            world.addTile(position, {1});
        } else {
            world.removeTile(position);
        }
    }

    int speed = 5;
    if (up) player_position.y -= speed;
    if (down) player_position.y += speed;
    if (left) player_position.x -= speed;
    if (right) player_position.x += speed;
}

void Game::poll() {
    Vector2f mousePosition;
    (void) SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
    INPUT_STATE.setMousePosition(mousePosition);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        bool keyUp = event.type == SDL_EVENT_KEY_UP;
        bool keyDown = event.type == SDL_EVENT_KEY_DOWN;
        
        switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                INPUT_STATE.setMouseDown(true);

                if (event.button.button == SDL_BUTTON_LEFT)
                    INPUT_STATE.setMouseSide(true);
                else if (event.button.button == SDL_BUTTON_RIGHT)
                    INPUT_STATE.setMouseSide(false);
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                INPUT_STATE.setMouseDown(false);
                break;
        }

        if (event.type == SDL_EVENT_KEY_DOWN && event.key.repeat == 0) {
            if (event.key.key == SDLK_W || event.key.key == SDLK_SPACE) up = true;
            if (event.key.key == SDLK_S) down = true;
            if (event.key.key == SDLK_A) left = true;
            if (event.key.key == SDLK_D) right = true;
        }

        if (event.type == SDL_EVENT_KEY_UP) {
            if (event.key.key == SDLK_W || event.key.key == SDLK_SPACE) up = false;
            if (event.key.key == SDLK_S) down = false;
            if (event.key.key == SDLK_A) left = false;
            if (event.key.key == SDLK_D) right = false;
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