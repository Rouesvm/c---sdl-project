#include <string>

#include "Game.hpp"

#include "math/Math.hpp"
#include "math/Vectors.hpp"
#include "renderer/Context.hpp"
#include "world/objects/Tile.hpp"

#include "world/World.hpp"

World::World() {
    tile_settings.push_back(TileSettings{});

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

    tile_settings.push_back(TileSettings{});

    tile_settings.push_back(TileSettings{
        true,
        false,
        1, 1,
        2,
        {TileIO{TYPE::INPUT, SIDE::DOWN, 0, 1}, TileIO{TYPE::OUTPUT, SIDE::UP, 0, 0}}
    });

    machine_type.push_back({});
    machine_type.push_back({});
    machine_type.push_back({});
    machine_type.push_back({});

    machine_type.push_back({
        [](double deltaTime, Machine& machine) {
            machine.ticks += deltaTime;
            if (machine.ticks >= 1) {
                machine.ticks = 0;

                Resource& firstSlot = machine.slots.front();
                firstSlot.amount += 1;
            }
        }
    });
}

Tile* World::getTile(const Vector2i& position) {
    auto it = tiles.find(position);
    if (it == tiles.end())
        return nullptr;
    return &it->second;
}

Tile* World::getMainTile(Vector2i* position) {
    const Vector2i main{*position};

    Tile* tile = getTile(main);
    if (tile == nullptr) 
        return nullptr;

    if (tile->isMultiTile()) {
        Vector2i newPosition = tile->getMainTile(main);
        position->x = newPosition.x;
        position->y = newPosition.y;
    }

    auto it = tiles.find(*position);
    if (it == tiles.end())
        return tile;

    return &it->second;
}

const Machine* World::getMachine(const Vector2i& position) {
    Vector2i mainPosition = position;
    Tile* main = getMainTile(&mainPosition);
    if (main == nullptr) 
        return nullptr;

    auto it = machines.find(mainPosition);
    if (it == nullptr) 
        return nullptr;

    return &it->second;
}

void World::addTile(const Vector2i& position, Tile tile) {
    auto main = tiles.find(position);
    if (main != tiles.end() && main->second.isSolid())
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
        const MachineType& machineType = tile.id < tile_settings.size() ? machine_type[tile.id] : machine_type[0];
        auto& machine = machines.emplace(position, setting.inventory_size).first->second;
        if (machineType.update != nullptr)
            machine.update = machineType.update;
    }
}

void World::removeTile(const Vector2i& position) {

    Vector2i main = position;
    const Tile* tile = getMainTile(&main);
    if (tile == nullptr)
        return;

    int mainID = tile->id;
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
        TextContext text{ std::to_string(tile.rotation), {static_cast<int>(renderContext.dst.x), static_cast<int>(renderContext.dst.y)} };
        renderer.renderText(text);

        renderContext.src.x = 0;
        renderContext.rotation = tile.rotation * 90;
        
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
            renderContext.rotation = 0;
            renderContext.flipped = false;

            estimatedRenderedTiles++;
            renderer.renderTexture(dirt, renderContext);

            World::renderTile(renderer, renderContext, position);
        } 
    }

    ticking_machines = machines.size();
    estimated_rendered_tiles = estimatedRenderedTiles;
}   

void World::update(double deltaTime) {
    for (auto& [position, machine] : machines) {
        if (machine.update != nullptr) {
            machine.update(deltaTime, machine);
        }
    }
}
