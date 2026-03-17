#include <string>
#include <thread>

#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_events.h"

#include "management/AssetManager.hpp"

#include "math/Math.hpp"
#include "math/Vectors.hpp"

#include "player/Camera.hpp"
#include "renderer/Context.hpp"
#include "renderer/Renderer.hpp"
#include "world/objects/Tile.hpp"

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
    ASSET_MANAGER.insertTexture("convenyor", "asset/conveyor-2.png");
    ASSET_MANAGER.insertTexture("drill", "asset/furnace.png");
    int finalSize = ASSET_MANAGER.storedTextureSize();

    for (int i = 0; i < finalSize - start; i++) {
        world.textures[i] = start + i;
    }
}

void Game::render(double deltaTime) {
    (void) deltaTime;

    window_renderer.clear();

    Renderer& renderer = window_renderer.currentRenderer();
    world.render(renderer);

    const Texture* texture;
    if (INPUT_STATE.isMouseDown()) {
        texture = ASSET_MANAGER.getTexture("pickaxe_clicked");
    } else texture = ASSET_MANAGER.getTexture("pickaxe");

    const Vector2f tilePosition = Math::floorDiv(
        INPUT_STATE.mousePosition()
            .divide(renderer.zoom)
            .add(CLIENT_STATE.cameraPosition()), 
        Game::TILE_SIZE_IN_PIXELS
    );

    const Vector2i tileIntPosition = Math::toRoundedVector2i(tilePosition);
    const Machine* machine = world.getMachine(tileIntPosition);
    
    if (machine != nullptr) {
        std::string string;
        string += " x: " + std::to_string(tileIntPosition.x); 
        string += " y: " + std::to_string(tileIntPosition.y);

        int machineSize = static_cast<int>(machine->slots.size());
        for (int i = 0; i < machineSize; i++) {
            string += "\n Slot " + std::to_string(i) + ":";

            const Resource& slot = machine->slots[i];
            string += "\n  Storage: " + std::to_string(slot.amount);
            string += "\n  Item ID: " + std::to_string(slot.item_id);
        }

        TextContext storageContext{
            string,
            Math::toRoundedVector2i(INPUT_STATE.mousePosition())
        };

        Vector2i squarePosition{Math::toRoundedVector2i(
            tilePosition
                .multiply(Game::TILE_SIZE_IN_PIXELS)
                .minus(CLIENT_STATE.cameraPosition())
                .multiply(renderer.zoom)
        )};

        SquareContext squareContext{
            false,
            Math::toVector2i(Vector2f{
                renderer.zoom * Game::TILE_SIZE_IN_PIXELS,
                renderer.zoom * Game::TILE_SIZE_IN_PIXELS
            }),
            squarePosition,
            2
        };

        renderer.renderSquare(squareContext);
        renderer.renderText(storageContext);
    }

    std::string debugText;
    debugText += "FPS: " + std::to_string(average_fps) + '\n';
    debugText += "Rotation: " + std::to_string(INPUT_STATE.currentRotation()) + '\n';
    debugText += "Rendered Tiles: " + std::to_string(world.estimated_rendered_tiles) + '\n';
    debugText += "Ticking Machines: " + std::to_string(world.ticking_machines);
    TextContext debugContext{
        debugText,
        {0, 0}
    };

    renderer.renderText(debugContext);
    renderer.renderTexture(texture, position, PLAYER_SIZE);
    renderer.render();

    window_renderer.present();
}

void Game::update(double deltaTime) {
    window_renderer.update();
    position = INPUT_STATE.mousePosition() - HALF_SIZE;

    camera.update(CLIENT_STATE.windowSize(), player_position, deltaTime);

    CLIENT_STATE.setCameraPosition(camera.screen_position);
    CLIENT_STATE.setWindowSize(window_renderer.windowSize());

    Renderer& renderer = window_renderer.currentRenderer();
    renderer.zoom = camera.zoom;

    world.update(deltaTime);

    const Vector2f tilePosition = Math::floorDiv(
        INPUT_STATE.mousePosition()
            .divide(renderer.zoom)
            .add(CLIENT_STATE.cameraPosition()),
        Game::TILE_SIZE_IN_PIXELS
    );

    if (INPUT_STATE.isMouseDown()) {
        const Vector2i tileIntPosition = Math::toVector2i(tilePosition);
        if (!INPUT_STATE.isLeft()) {
            world.addTile(tileIntPosition, {static_cast<uint16_t>(block_id), 0, INPUT_STATE.currentRotation()});
        } else world.removeTile(tileIntPosition);
    }

    int speed = 1;
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
            case SDL_EVENT_KEY_DOWN:
                INPUT_STATE.setKeyDown(true);
                INPUT_STATE.setKeyUp(false);
                break;
            case SDL_EVENT_KEY_UP:
                INPUT_STATE.setKeyDown(false);
                INPUT_STATE.setKeyUp(true);
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                const int& scrollY = event.wheel.y;
                if (scrollY < 0) {
                    if (camera.zoom < 4.0f) camera.zoom += 0.5f;
                } else if (scrollY > 0) {
                    if (camera.zoom > 1.0f) camera.zoom -= 0.5f;
                }
                break;
        }

        switch (event.key.key) {
            case SDLK_W: up = INPUT_STATE.isKeyPressed(); break;
            case SDLK_S: down = INPUT_STATE.isKeyPressed(); break;
            case SDLK_A: left = INPUT_STATE.isKeyPressed(); break;
            case SDLK_D: right = INPUT_STATE.isKeyPressed(); break;
            case SDLK_R: if (event.key.repeat == 0 && INPUT_STATE.isKeyPressed()) INPUT_STATE.setRotation((INPUT_STATE.currentRotation() + 1) % 4); break;
        }

        if (INPUT_STATE.isKeyPressed()) {
            switch (event.key.key) {
                case SDLK_1: block_id = 1; break;
                case SDLK_2: block_id = 3; break;
                case SDLK_3: block_id = 4; break;
            }
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

    int framesRendered = 0;
    double fpsTimer = 0.0;

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
        this->render(alpha);

        framesRendered++;
        if (static_cast<double>(frameStart - fpsTimer) / frequency >= 1.0) {
            average_fps = framesRendered;
            framesRendered = 0;
            fpsTimer = frameStart;
        }
        
        const double frameTime = static_cast<double>(SDL_GetPerformanceCounter() - frameStart) / frequency;
        if (FRAME_DURATION > 0 && frameTime < FRAME_DURATION) {
            preciseSleep(FRAME_DURATION - frameTime);
        }  
    }

    (void) SDL_Quit();
}