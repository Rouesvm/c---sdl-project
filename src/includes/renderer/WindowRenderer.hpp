#include <string>

#include <SDL3/SDL_video.h>

#include "Renderer.hpp"

class WindowRenderer {
    private:
        Renderer renderer;
        SDL_Window* window;
    public:
        WindowRenderer(const std::string&);
    public:
        void clear();
        void present();
    public:
        Renderer& currentRenderer();
};