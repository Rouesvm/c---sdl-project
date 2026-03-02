#include "math/Vectors.hpp"
#include "renderer/WindowRenderer.hpp"
#include "management/AssetManager.hpp"

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
    private:
        static InputState INPUT_STATE;

        WindowRenderer window_renderer;
        AssetManager asset_manager;

        bool running = true;
    public:
        Game();
    public:
        static const InputState& inputState() {return INPUT_STATE;};
    private:
        void render();
        void poll();
        void update();
        void tick();
    public:
        void loop();
};