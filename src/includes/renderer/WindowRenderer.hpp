#include <string>

#include <SDL3/SDL_video.h>

#include "Renderer.hpp"
#include "TextRenderer.hpp"

#include "math/Vectors.hpp"

class WindowRenderer {
    private:
        Renderer renderer;
        TextRenderer text_renderer;

        SDL_Window* window;

        Vector2i window_size;
    public:
        WindowRenderer(const std::string&);
    public:
        void clear();
        void present();
        void update();
    public:
        Renderer& currentRenderer() {return renderer;}
        const Vector2i& windowSize() const {return window_size;}
};