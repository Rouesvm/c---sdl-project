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
    tile_settings.push_back(TileSettings{
        true,
        true,
        2, 2,
        2,
        {TileIO{TYPE::INPUT, SIDE::DOWN, 0, 1}, TileIO{TYPE::OUTPUT, SIDE::UP, 0, 0}}
    });
    tile_settings.push_back(TileSettings{
    });
}

void World::addTile(const Vector2i& position, Tile tile) {
   auto it = tiles.find(position);
    if (it != tiles.end() && it->second.isSolid())
        return;

    Tile& current = tiles[position];

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
    if (setting.is_machine) {
        machines.emplace(position, setting.inventory_size);
    }
}

void World::removeTile(const Vector2i& position) {
    auto it = tiles.find(position);
    if (it == tiles.end())
        return;

    Vector2i main = position;

    if (it->second.isMultiTile())
        main = it->second.getMainTile(position);

    it = tiles.find(main);
    if (it == tiles.end())
        return;

    int mainID = it->second.id;
    const TileSettings& setting = mainID < tile_settings.size() ? tile_settings[mainID] : tile_settings[0];
    if (setting.is_multi_tiled) {
        for (int x = 0; x < setting.size_x; x++) {
            for (int y = 0; y < setting.size_y; y++) {
                tiles.erase({main.x + x, main.y + y});
            }
        }
    } else tiles.erase(main);

    if (setting.is_machine) {
        machines.erase(main);
    }
}

void World::renderTile(Renderer& renderer, RenderContext& renderContext, const Vector2i& position) {
    const auto& result = tiles.find(position);
    if (result == tiles.end()) return;
    const Tile& tile = result->second;

    if (tile.isAir()) return;

    const Texture* blockTexture = Game::assetManager().getTexture(textures[tile.id]);
    if (tile.id == 3) {
        Vector2i coords[4] = {
            { position.x,   position.y-1 }, // up
            { position.x,   position.y+1 },  // down
            { position.x-1, position.y   }, // left
            { position.x+1, position.y   } // right
        };

        bool A = tiles.contains(coords[0]) && tiles.at(coords[0]).id == tile.id;
        bool B = tiles.contains(coords[1]) && tiles.at(coords[1]).id == tile.id;
        bool C = tiles.contains(coords[2]) && tiles.at(coords[2]).id == tile.id;
        bool D = tiles.contains(coords[3]) && tiles.at(coords[3]).id == tile.id;

        int mask = 0;
        switch (tile.rotation) {
            case 0: A = true; B = false; break;
            case 1: D = true; C = false; break;
            case 2: B = true; A = false; break;
            case 3: C = true; D = false; break;
        }

        if (A) mask |= 1;
        if (B) mask |= 2;
        if (C) mask |= 4;
        if (D) mask |= 8;

        const Vector2i& src = World::CONVEYOR_SRC_POSITIONS[mask];

        TextContext text{std::to_string(mask), {static_cast<int>(renderContext.dst.x), static_cast<int>(renderContext.dst.y)}};
        renderer.renderText(text);

        renderContext.src.x = src.x * 16;
        renderContext.src.y = src.y * 16;
            
        renderer.renderTexture(blockTexture, renderContext);
    } else if (tile.id != 0) {
        renderer.renderTexture(blockTexture, renderContext);
    } else {
        Vector2i tileOffset = tile.getOffset();
        const auto& it = tiles.find(position - tileOffset);
        if (it == tiles.end()) return;;

        int mainTileID = it->second.id;

        const Texture* blockTexture = Game::assetManager().getTexture(textures[mainTileID]);
        renderContext.src.x = tileOffset.x * 16;
        renderContext.src.y = tileOffset.y * 16;
        renderer.renderTexture(blockTexture, renderContext);
    }
}

void World::render(Renderer& renderer) {
    const AssetManager& manager = Game::assetManager();

    int estimatedRenderedTiles = 0;

    float factor = Game::TILE_SIZE_IN_PIXELS * renderer.zoom;
    const Vector2f& cameraOffset = Game::clientState().cameraPosition();

    const Vector2f startTiles = Math::floorDiv(cameraOffset, Game::TILE_SIZE_IN_PIXELS);
    const Vector2f endTiles = Math::floorDiv(cameraOffset + Math::toVector2f(Game::clientState().windowSize() / renderer.zoom), Game::TILE_SIZE_IN_PIXELS);

    const int startX = static_cast<int>(startTiles.x) - 1;
    const int startY = static_cast<int>(startTiles.y) - 1;
    const int endX = static_cast<int>(endTiles.x) + 1;
    const int endY = static_cast<int>(endTiles.y) + 1;

    RenderContext renderContext{
        .src = {0, 0, 16, 16},
        .dst = {0, 0, 
            factor, 
            factor
        }
    };

    Vector2i position{};

    const Texture* dirt = manager.getTexture("dirt");
    for (int x = startX; x <= endX; x++) {
        position.x = x;
        renderContext.dst.x = ((position.x * Game::TILE_SIZE_IN_PIXELS) - cameraOffset.x) * renderer.zoom;

        for (int y = startY; y <= endY; y++) {
            position.y = y;
            renderContext.dst.y = ((position.y * Game::TILE_SIZE_IN_PIXELS) - cameraOffset.y) * renderer.zoom;

            renderContext.src.x = 0;
            renderContext.src.y = 0;

            estimatedRenderedTiles++;
            renderer.renderTexture(dirt, renderContext);

            World::renderTile(renderer, renderContext, position);
        } 
    }

    TextContext text{std::to_string(estimatedRenderedTiles) + "\n" + "Ticking Machines: " + std::to_string(machines.size()), {0, 16}};
    renderer.renderText(text);
}   

void World::update(double deltaTime) {
    for (auto& [position, machine] : machines) {
        if (machine.update != nullptr) {
            machine.update(deltaTime);
        }
    }
}
