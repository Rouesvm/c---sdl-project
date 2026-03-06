#include <string>

#include "Game.hpp"

#include "math/Math.hpp"
#include "math/Vectors.hpp"
#include "renderer/Context.hpp"
#include "world/objects/Tile.hpp"

#include "world/World.hpp"

World::World() {
    is_machine.push_back(false);
    is_machine.push_back(true);
}

void World::addTile(const Vector2i& position, Tile tile) {
    auto& current = tiles[position];
    if (current.isSolid())
        return;

    if (is_machine.size() >= tile.id && is_machine[tile.id]) {
        Vector2i tileSize{2,2};

        for (int x = 0; x < tileSize.x; x++) {
            for (int y = 0; y < tileSize.y; y++) {
                if (x == 0 && y == 0) continue;

                const auto& it = tiles.find({position.x + x, position.y + y});
                if (it != tiles.end() && !it->second.isAir())
                    return;
            }
        }

        for (int x = 0; x < tileSize.x; x++) {
            for (int y = 0; y < tileSize.y; y++) {
                if (x == 0 && y == 0) continue;

                Vector2i offsetPosition{position.x + x, position.y + y};
                tiles[offsetPosition].setMainTile(position, offsetPosition);
            }
        }
    }

    current = std::move(tile);
}

void World::removeTile(const Vector2i& position) {
    auto it = tiles.find(position);
    if (it == tiles.end())
        return;

    Vector2i main = position;

    if (it->second.isMultiTile())
        main = it->second.getMainTile(position);

    int mainID = tiles[main].id;
    if (is_machine.size() >= mainID && is_machine[mainID]) {
        Vector2i size{2,2};

        for (int x = 0; x < size.x; x++) {
            for (int y = 0; y < size.y; y++) {
                tiles.erase({main.x + x, main.y + y});
            }
        }
    } else {
        tiles.erase(main);
    }
}

void World::render(Renderer& renderer) {
    Vector2f tileSize{Game::TILE_SIZE, Game::TILE_SIZE};
    Vector2i position;

    const AssetManager& manager = Game::assetManager();

    int renderedBlocks = 0;

    const Vector2i& windowSizeInTiles = Math::floorDiv(Game::clientState().windowSize(), Game::TILE_SIZE * renderer.zoom);

    RenderContext renderContext{};
    renderContext.src = {0, 0, 16, 16};

    Vector2f renderPosition;
    Vector2f renderSize;

    for (int x = 0; x < windowSizeInTiles.x + 1; x++) {
        for (int y = 0; y < windowSizeInTiles.y + 1; y++) {
            position.x = x;
            position.y = y;

            renderedBlocks++;
            renderer.renderTexture(manager.getTexture("dirt"), (Vector2f{position.x, position.y} * Game::TILE_SIZE) * renderer.zoom, tileSize * renderer.zoom);

            const auto& it = tiles.find(position);
            if (it == tiles.end()) continue;

            Tile& tile = it->second;

            if (tile.isAir()) continue;

            renderPosition = (Vector2f{position.x, position.y} * Game::TILE_SIZE) * renderer.zoom;
            renderSize = (tileSize) * renderer.zoom;

            renderContext.dst = {renderPosition.x, renderPosition.y, renderSize.x, renderSize.y};

            if (tile.id != 0) {
                const Texture* blockTexture = manager.getTexture(textures[tile.id]);
                renderContext.src.x = 0;
                renderContext.src.y = 0;
                renderer.renderTexture(blockTexture, renderContext);
            } else {
                Vector2i tileOffset = tile.getOffset();
                auto it = tiles.find(position - tileOffset);
                if (it == tiles.end()) continue;

                int mainTileID = it->second.id;

                const Texture* blockTexture = manager.getTexture(textures[mainTileID]);
                Vector2i renderOffset = tileOffset * 16;
                renderContext.src.x = renderOffset.x;
                renderContext.src.y = renderOffset.y;
                renderer.renderTexture(blockTexture, renderContext);
            }
        }
    }

    TextContext text{std::to_string(renderedBlocks), {0, 16}};
    renderer.renderText(text);
}   

void World::update() {

}