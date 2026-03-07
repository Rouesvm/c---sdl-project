#include <string>

#include "Game.hpp"

#include "math/Math.hpp"
#include "math/Vectors.hpp"
#include "renderer/Context.hpp"
#include "world/objects/Tile.hpp"

#include "world/World.hpp"

World::World() {
    tile_settings.push_back(TileSettings{
    });
    tile_settings.push_back(TileSettings{
        true,
        true,
        2, 2,
        2,
        {TileIO{TYPE::INPUT, SIDE::DOWN, 0, 1}, TileIO{TYPE::OUTPUT, SIDE::UP, 0, 0}}
    });
}

void World::addTile(const Vector2i& position, Tile tile) {
    auto& current = tiles[position];
    if (current.isSolid())
        return;

    const TileSettings& setting = tile.id < tile_settings.size() ? tile_settings[tile.id] : tile_settings[0];
    if (setting.is_multi_tiled) {
        for (int x = 0; x < setting.size_x; x++) {
            for (int y = 0; y < setting.size_y; y++) {
                if (x == 0 && y == 0) continue;

                const auto& it = tiles.find({position.x + x, position.y + y});
                if (it != tiles.end() && !it->second.isAir())
                    return;
            }
        }

        for (int x = 0; x < setting.size_x; x++) {
            for (int y = 0; y < setting.size_y; y++) {
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
    const TileSettings& setting = mainID < tile_settings.size() ? tile_settings[mainID] : tile_settings[0];
    if (setting.is_multi_tiled) {
        for (int x = 0; x < setting.size_x; x++) {
            for (int y = 0; y < setting.size_y; y++) {
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

    float factor = Game::TILE_SIZE_IN_PIXELS * renderer.zoom;
    const Vector2f& cameraOffset = Game::clientState().cameraPosition();

    const Vector2f startTiles = Math::floorDiv(cameraOffset, Game::TILE_SIZE_IN_PIXELS);
    const Vector2f endTiles = Math::floorDiv(cameraOffset + Math::toVector2f(Game::clientState().windowSize() / renderer.zoom), Game::TILE_SIZE_IN_PIXELS);

    const Vector2i start = Math::toVector2i((startTiles)) - Vector2i{1, 1};
    const Vector2i end = Math::toVector2i((endTiles)) + Vector2i{1, 1}; 

    RenderContext renderContext{
        .src = {0, 0, 16, 16},
        .dst = {0, 0, 
            factor, 
            factor
        }
    };

    Vector2i position{};
    Vector2i tileOffset{};

    const Texture* dirt = manager.getTexture("dirt");
    for (int x = start.x; x <= end.x; x++) {
        position.x = x;
        renderContext.dst.x = ((position.x * Game::TILE_SIZE_IN_PIXELS) - cameraOffset.x) * renderer.zoom;

        for (int y = start.y; y <= end.y; y++) {
            position.y = y;
            renderContext.dst.y = ((position.y * Game::TILE_SIZE_IN_PIXELS) - cameraOffset.y) * renderer.zoom;

            renderContext.src.x = 0;
            renderContext.src.y = 0;

            estimatedRenderedTiles++;
            renderer.renderTexture(dirt, renderContext);

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