#include "math/Vectors.hpp"

#include "renderer/WindowRenderer.hpp"
#include "management/AssetManager.hpp"
#include "world/World.hpp"

class InputState {
    bool MOUSE_DOWN;
    Vector2f MOUSE_POSITION;
    public:
        const Vector2f& mousePosition() const {return MOUSE_POSITION;}
        bool isMouseDown() const {return MOUSE_DOWN;}
    public:
        void setMousePosition(Vector2f& position) {MOUSE_POSITION = position;};
        void setMouseDown(bool isDown) {MOUSE_DOWN = isDown;};
};

class Game {
    public:
        static constexpr inline double PHYSICS_STEP = 1 / 60.0;
        static constexpr inline double FRAME_DURATION = 1 / 60.0;

        static constexpr Vector2f PLAYER_SIZE{8, 8};
        static constexpr Vector2f HALF_SIZE{PLAYER_SIZE.x / 2, PLAYER_SIZE.y / 2};
    private:
        static InputState INPUT_STATE;
        static AssetManager ASSET_MANAGER;

        WindowRenderer window_renderer;

        World world;

        bool running = true;

        Vector2f position;
    public:
        Game();
    public:
        static const InputState& inputState() {return INPUT_STATE;};
        static const AssetManager& assetManager() {return ASSET_MANAGER;};
    private:
        void render();
        void poll();
        void update(double deltaTime);
        void tick(double deltaTime);
    public:
        void loop();
};