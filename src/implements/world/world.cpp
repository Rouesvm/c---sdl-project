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
    const AssetManager& manager = Game::assetManager();

    int estimatedRenderedTiles = 0;

    const Vector2i& windowSizeInTiles = Math::floorDiv(Game::clientState().windowSize(), Game::TILE_SIZE * renderer.zoom);

    float zoom = Game::TILE_SIZE * renderer.zoom;
    RenderContext renderContext{
        .src = {0, 0, 16, 16},
        .dst = {0, 0, 
            zoom, 
            zoom
        }
    };

    Vector2i position{};
    Vector2i tileOffset{};

    for (int x = 0; x < windowSizeInTiles.x + 1; x++) {
        position.x = x;
        renderContext.dst.x = position.x * zoom;

        for (int y = 0; y < windowSizeInTiles.y + 1; y++) {
            position.y = y;
            renderContext.dst.y = position.y * zoom;

            renderContext.src.x = 0;
            renderContext.src.y = 0;

            estimatedRenderedTiles++;
            renderer.renderTexture(manager.getTexture("dirt"), renderContext);

            const auto& it = tiles.find(position);
            if (it == tiles.end()) continue;
            const Tile& tile = it->second;

            if (tile.isAir()) continue;

            if (tile.id != 0) {
                const Texture* blockTexture = manager.getTexture(textures[tile.id]);
                renderer.renderTexture(blockTexture, renderContext);
            } else {
                tileOffset = tile.getOffset();
                const auto& it = tiles.find(position - tileOffset);
                if (it == tiles.end()) continue;

                int mainTileID = it->second.id;

                const Texture* blockTexture = manager.getTexture(textures[mainTileID]);
                renderContext.src.x = tileOffset.x * 16;
                renderContext.src.y = tileOffset.y * 16;
                renderer.renderTexture(blockTexture, renderContext);
            }
        }
    }

    TextContext text{std::to_string(estimatedRenderedTiles), {0, 16}};
    renderer.renderText(text);
}   

void World::update() {

}