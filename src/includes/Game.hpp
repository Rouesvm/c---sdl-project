#include "math/Vectors.hpp"

#include "renderer/WindowRenderer.hpp"
#include "management/AssetManager.hpp"
#include "player/Camera.hpp"
#include "world/World.hpp"

class InputState {
    bool IS_LEFT;
    bool MOUSE_DOWN;
    Vector2f MOUSE_POSITION;
    public:
        const Vector2f& mousePosition() const {return MOUSE_POSITION;}
        bool isMouseDown() const {return MOUSE_DOWN;}
        bool isLeft() const {return IS_LEFT;}
    public:
        void setMousePosition(Vector2f& position) {MOUSE_POSITION = position;};
        void setMouseDown(bool isDown) {MOUSE_DOWN = isDown;};
        void setMouseSide(bool isLeft) {IS_LEFT = isLeft;};
};

class ClientState {
    Vector2i WINDOW_SIZE;
    Vector2f CAMERA_POSITION;
    public:
        const Vector2i& windowSize() const {return WINDOW_SIZE;}
        const Vector2f& cameraPosition() const {return CAMERA_POSITION;}
    public:
        void setWindowSize(const Vector2i& position) {WINDOW_SIZE = position;};
        void setCameraPosition(const Vector2f& position) {CAMERA_POSITION = position;};
};

class Game {
    public:
        static int inline TILE_SIZE_IN_PIXELS = 8;

        static constexpr inline double PHYSICS_STEP = 1 / 60.0;
        static constexpr inline double FRAME_DURATION = 1 / 60.0;

        static constexpr Vector2f PLAYER_SIZE{8, 8};
        static constexpr Vector2f HALF_SIZE{PLAYER_SIZE.x / 2, PLAYER_SIZE.y / 2};
    private:
        static InputState INPUT_STATE;
        static ClientState CLIENT_STATE;
        static AssetManager ASSET_MANAGER;

        WindowRenderer window_renderer;

        Camera camera;
        World world;

        bool running = true;

        Vector2f position;
        Vector2f player_position;

        bool up = false;
        bool down = false;
        bool left = false;
        bool right = false;
    public:
        Game();
    public:
        static const InputState& inputState() {return INPUT_STATE;};
        static const ClientState& clientState() {return CLIENT_STATE;};
        static const AssetManager& assetManager() {return ASSET_MANAGER;};
    private:
        void render();
        void poll();
        void update(double deltaTime);
        void tick(double deltaTime);
    public:
        void loop();
};