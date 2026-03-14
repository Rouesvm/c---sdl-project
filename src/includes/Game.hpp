#include "math/Vectors.hpp"

#include "renderer/WindowRenderer.hpp"
#include "management/AssetManager.hpp"
#include "player/Camera.hpp"
#include "world/World.hpp"

class InputState {
    Vector2f MOUSE_POSITION;

    bool IS_LEFT : 1;
    bool MOUSE_DOWN : 1;

    bool KEY_UP : 1;
    bool KEY_DOWN : 1;

    uint8_t rotation : 2 = 0;

    public:
        const Vector2f& mousePosition() const {return MOUSE_POSITION;}
        bool isMouseDown() const {return MOUSE_DOWN;}
        bool isLeft() const {return IS_LEFT;}
        bool isKeyUp() const {return KEY_UP;};
        bool isKeyDown() const {return KEY_DOWN;};
        bool isKeyPressed() const {return isKeyDown() && !isKeyUp();};
        uint8_t currentRotation() const {return rotation;};
    public:
        void setMousePosition(Vector2f& position) {MOUSE_POSITION = position;};
        void setMouseDown(bool isDown) {MOUSE_DOWN = isDown;};
        void setMouseSide(bool isLeft) {IS_LEFT = isLeft;};
        void setKeyUp(bool isKeyUp) {KEY_UP = isKeyUp;};
        void setKeyDown(bool isKeyDown) {KEY_DOWN = isKeyDown;};
        void setRotation(uint8_t currentRotation) {rotation = currentRotation;};
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
        static int inline TILE_SIZE_IN_PIXELS = 16;

        static constexpr inline double PHYSICS_STEP = 1 / 60.0;
        static constexpr inline double FRAME_DURATION = 1 / 60.0;

        static constexpr Vector2f PLAYER_SIZE{64, 64};
        static constexpr Vector2f HALF_SIZE{PLAYER_SIZE.x / 2, PLAYER_SIZE.y / 2};
    private:
        static InputState INPUT_STATE;
        static ClientState CLIENT_STATE;
        static AssetManager ASSET_MANAGER;
    private:
        WindowRenderer window_renderer;

        Camera camera;
        World world;
    private:
        bool running = true;

        Vector2f position;
        Vector2f player_position;

        bool up = false;
        bool down = false;
        bool left = false;
        bool right = false;

        int block_id = 1;
        int average_fps = 0;
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