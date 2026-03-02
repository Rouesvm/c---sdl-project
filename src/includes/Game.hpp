#include "renderer/WindowRenderer.hpp"
#include "management/AssetManager.hpp"

class Game {
    private:
        AssetManager asset_manager;
        WindowRenderer window_renderer;
        bool running = true;
    public:
        Game();
    private:
        void render();
        void poll();
        void update();
        void tick();
    public:
        void loop();
};