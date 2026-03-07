#include "SDL3/SDL_render.h"

#include "SDL3_ttf/SDL_ttf.h"
#include "math/Vectors.hpp"
#include "renderer/Context.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/TextRenderer.hpp"

TextSurface::TextSurface(Renderer& currentRenderer, TTF_Font* ttfFont) {
    this->renderer = &currentRenderer;

    this->font = ttfFont;
    
    this->line_height = TTF_GetFontHeight(ttfFont);
    this->font_size = TTF_GetFontSize(font);
};

TextSurface::~TextSurface() {
    for (auto& [_, glyph] : glyphs) {
        if (glyph.gpu_char) SDL_DestroyTexture(glyph.gpu_char);
    }
}

Glyph& TextSurface::getGlyph(char c) {
    auto it = glyphs.find(c);
    if (it != glyphs.end()) return it->second;

    Glyph glyph;
    glyph.gpu_char = nullptr;

    int minx, maxx, miny, maxy, advance;
    if (TTF_GetGlyphMetrics(font, c, &minx, &maxx, &miny, &maxy, &advance)) {
        SDL_Surface* surface = TTF_RenderGlyph_Blended(font, c, SDL_Color{255, 255, 255, 255});
        if (!surface) {
            std::cerr << "TTF error: " << SDL_GetError() << "\n";
            glyph.advance = 10;
        } else {
            glyph.gpu_char = SDL_CreateTextureFromSurface(renderer->getRenderer(), surface);
            SDL_DestroySurface(surface);

            if (!glyph.gpu_char) {
                std::cerr << "SDL error: " << SDL_GetError() << "\n";
            }

            SDL_SetTextureScaleMode(glyph.gpu_char, SDL_SCALEMODE_NEAREST);

            float w, h;
            SDL_GetTextureSize(glyph.gpu_char, &w, &h);
            glyph.width = w;
            glyph.height = h;

            glyph.advance = advance;
        }
    } else {
        glyph.advance = 10;
    }

    auto [newIt, inserted] = glyphs.emplace(c, std::move(glyph));
    return newIt->second;
}

void TextSurface::renderText(const TextContext& context) {
    Vector2f renderPosition{context.position.x, context.position.y};
    Vector2f size{0, 0};

    Texture texture{nullptr};
    for (const char& c : context.text) {
        Glyph& glyph = getGlyph(c);
        if (glyph.gpu_char) {
            texture.texture = glyph.gpu_char;

            size.x = glyph.width;
            size.y = glyph.height;
            renderer->renderTexture(&texture, renderPosition, size);

            renderPosition.x += glyph.advance;
        }
    }
}

TextRenderer::TextRenderer(Renderer& currentRenderer) {
    if (!TTF_Init()) {
        std::cerr << "Error when initializing TTF. " << SDL_GetError() << '\n';
        return;
    }

    TTF_Font* ttfFont = TTF_OpenFont("asset/font.ttf", 16);
    if (!ttfFont) {
        std::cerr << "Error when creating TTF_FONT" << SDL_GetError() << '\n';
        return;
    }

    this->font = ttfFont;
    text = TextSurface(currentRenderer, font);
};

TextRenderer::~TextRenderer() {

}

void TextRenderer::addText(TextContext& context) {
    to_render.push_back(context);
}

void TextRenderer::render() {
    if (!font) return;
    
    for (TextContext& context : to_render) {
        text.renderText(context);
    }

    to_render.clear();
}