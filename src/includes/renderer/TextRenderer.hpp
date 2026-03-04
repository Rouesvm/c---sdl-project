#include <cstdint>
#include <unordered_map>
#include <vector>

#include "SDL3_ttf/SDL_ttf.h"

#include "renderer/Context.hpp"
#include "renderer/Renderer.hpp"

struct Glyph {
    SDL_Texture* gpu_char;
    int advance = 0;
    int width = 0;
    int height = 0;
};

class TextSurface {
    private:
        std::unordered_map<char, Glyph> glyphs{};

        TTF_Font* font = nullptr; 
        Renderer* renderer = nullptr;
    private:
        int line_height = 0;
        int font_size = 16;
    public:
        TextSurface() {};
        TextSurface(Renderer&, TTF_Font*);

        ~TextSurface();

        Glyph& getGlyph(char c);
        void renderText(const TextContext&);
};

class TextRenderer {
    private:
        std::vector<TextContext> to_render;
    private:
        TextSurface text;

        TTF_Font* font = nullptr; 
        Renderer* renderer = nullptr;
    public:
        TextRenderer() {};
        TextRenderer(Renderer&);

        ~TextRenderer();
    public:
        void addText(TextContext&);
        void render();
};