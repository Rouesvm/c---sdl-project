#include "SDL3/SDL_render.h"

#include "SDL3_ttf/SDL_ttf.h"
#include "math/Vectors.hpp"
#include "renderer/Context.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/TextRenderer.hpp"

TextSurface::TextSurface(Renderer& currentRenderer, TTF_Font* ttfFont, TTF_TextEngine* textEngine) {
    this->renderer = &currentRenderer;

    this->font = ttfFont;
    this->text_engine = textEngine;
    
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
    glyph.gpu_char = {};

    int minx, maxx, miny, maxy, advance;
    if (TTF_GetGlyphMetrics(font, c, &minx, &maxx, &miny, &maxy, &advance)) {
        SDL_Surface* surface = TTF_RenderGlyph_Blended(font, c, SDL_Color{255, 255, 255, 255});
        if (!surface) {
            std::cerr << "TTF error: " << SDL_GetError() << "\n";
            return glyphs[c];
        }

        glyph.gpu_char = SDL_CreateTextureFromSurface(renderer->getRenderer(), surface);
        SDL_SetTextureScaleMode(glyph.gpu_char, SDL_SCALEMODE_NEAREST);
        SDL_DestroySurface(surface);

        float w, h;
        SDL_GetTextureSize(glyph.gpu_char, &w, &h);
        glyph.width = w;
        glyph.height = h;

        if (!glyph.gpu_char) {
            std::cerr << "SDL error: " << SDL_GetError() << "\n";
            return glyphs[c];
        }

        glyph.advance = advance;
    } else {
        glyph.advance = 10;
    }

    glyphs[c] = glyph;
    return glyphs[c];
}

void TextSurface::renderText(const TextContext& context) {
    Vector2f renderPosition{context.position.x, context.position.y};
    for (const char& c : context.text) {
        Glyph& glyph = getGlyph(c);
        if (glyph.gpu_char) {
            Texture texture{glyph.gpu_char, {}};
            renderer->renderTexture(&texture, renderPosition, {glyph.width, glyph.height});
            
            renderPosition.x += glyph.advance;
        }
    }
}

TextRenderer::TextRenderer(Renderer& currentRenderer) {
    if (!TTF_Init()) {
        std::cerr << "Error when initializing TTF. " << SDL_GetError() << '\n';
        return;
    }

    TTF_TextEngine* textEngine = TTF_CreateRendererTextEngine(currentRenderer.getRenderer());
    if (!textEngine) {
        std::cerr << "Error when creating TTF_TEXTENGINE. " << SDL_GetError() << '\n';
        return;
    }

    TTF_Font* ttfFont = TTF_OpenFont("asset/font.ttf", 16);
    if (!ttfFont) {
        std::cerr << "Error when creating TTF_FONT" << SDL_GetError() << '\n';
        return;
    }

    this->font = ttfFont;
    this->text_engine = textEngine;

    text = TextSurface(currentRenderer, font, text_engine);
};

void TextRenderer::addText(TextContext& context) {
    to_render.push_back(context);
}

void TextRenderer::render() {
    for (TextContext& context : to_render) {
        text.renderText(context);
    }

    to_render.clear();
}