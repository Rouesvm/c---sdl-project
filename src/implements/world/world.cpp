#include <cstdint>
#include <string>

#include "Game.hpp"

#include "math/Math.hpp"
#include "math/Vectors.hpp"
#include "renderer/Context.hpp"
#include "world/objects/Tile.hpp"

#include "world/World.hpp"

static constexpr Vector2i DIR[4] = { {0,-1}, {1,0}, {0,1}, {-1,0} };

World::World() {
    tile_settings.push_back(TileSettings{});

    tile_settings.push_back(TileSettings{
        .is_machine = true,
        .is_multi_tiled = true,
        .size_x = 2, .size_y = 2,
        .inventory_size = 2,
        .ios = {TileIO{TYPE::INPUT, SIDE::DOWN, 0, 1}, TileIO{TYPE::OUTPUT, SIDE::UP, 0, 0}}
    });

    tile_settings.push_back(TileSettings{
        .is_machine = true,
        .is_multi_tiled = true,
        .size_x = 2, .size_y = 2,
        .inventory_size = 2,
        .ios = {TileIO{TYPE::INPUT, SIDE::DOWN, 0, 1}, TileIO{TYPE::OUTPUT, SIDE::UP, 0, 0}}
    });

    tile_settings.push_back(TileSettings{
        .is_machine = true,
        .inventory_size = 1,
        .ios = {},
        .machine_type = "conveyor"
    });

    tile_settings.push_back(TileSettings{
        .is_machine = true,
        .inventory_size = 2,
        .ios = {TileIO{TYPE::INPUT, SIDE::DOWN, 0, 1}, TileIO{TYPE::OUTPUT, SIDE::UP, 0, 0}},
        .machine_type = "drill"
    });

    machine_logic[""] = {[](double deltaTime, World& world, Machine& machine) {}};
    machine_logic["conveyor"] = {
        [](double deltaTime, World& world, Machine& machine) {
            const Tile* tile = world.getTile(machine.position);
            if (tile == nullptr) return;
            machine.ticks += deltaTime;

            const Vector2i& offset = DIR[tile->rotation];
            const Vector2i offsetedPosition = machine.position.add(offset);

            Machine* insert = world.getMachine(offsetedPosition);

            if (machine.ticks < 1) return;

            machine.ticks = 0;
            if (insert == nullptr) return;

            Resource& insertSlot = insert->slots.front();
            Resource& thisSlot = machine.slots.front();
            if (thisSlot.amount > 0 && insertSlot.amount < insertSlot.max_size) {
                insertSlot.add(thisSlot.item_id, 1);
                thisSlot.remove(1);
            }
        }
    };
    machine_logic["drill"] = {
        [](double deltaTime, World& world, Machine& machine) {
            machine.ticks += deltaTime;
            if (machine.ticks < 1) return;
            machine.ticks = 0;

            int item_id = 1;

            Resource& thisSlot = machine.slots.front();
            thisSlot.add(item_id, 1);

            for (int i = 0; i < 4; i++) {
                const Vector2i& offset = DIR[i];
                const Vector2i offsetedPosition = machine.position.add(offset);

                Machine* insert = world.getMachine(offsetedPosition);
                if (!insert) continue;

                Resource& insertSlot = insert->slots.front();
                if (thisSlot.amount > 0 && insertSlot.amount < insertSlot.max_size) {
                    insertSlot.add(item_id, 1);
                    thisSlot.remove(1);
                }
            }
        }
    };
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

Machine* World::getMachine(const Vector2i& position) {
    Vector2i mainPosition = position;
    Tile* main = getMainTile(&mainPosition);
    if (main == nullptr) 
        return nullptr;

    auto it = machines.find(mainPosition);
    if (it == nullptr) 
        return nullptr;

    return &it->second;
}

const Machine* World::getConstMachine(const Vector2i& position) {
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
        const MachineLogic& machineType = machine_logic[setting.machine_type];
        auto& machine = machines.emplace(position, setting.inventory_size).first->second;
        machine.position = position;
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

static int rotateIndex(int localIndex, int rot) {
    return (localIndex + rot) & 3;
}

static bool tileConnects(World& world, Vector2i position, Vector2i myOut, int neighbourIndex, int tileId) {    
    Vector2i tilePos = { position.x + DIR[neighbourIndex].x,
                      position.y + DIR[neighbourIndex].y };

    Tile* tile = world.getTile(tilePos);
    if (!tile) return false;

    Vector2i nOut = { tilePos.x + DIR[tile->rotation].x,
                      tilePos.y + DIR[tile->rotation].y };

    if (nOut.x == position.x && nOut.y == position.y) return true;
    if (myOut.x == tilePos.x && myOut.y == tilePos.y) return true;
    return false;
}

void World::renderTile(Renderer& renderer, RenderContext& renderContext, const Vector2i& position) {
    const auto& result = tiles.find(position);
    if (result == tiles.end()) return;
    const Tile& tile = result->second;

    if (tile.isAir()) return;

    const Texture* blockTexture = Game::assetManager().getTexture(textures[tile.id]);
    if (tile.id == 3) {
        Vector2i myOut{ position.x + DIR[tile.rotation].x,
                        position.y + DIR[tile.rotation].y };

        bool behind = tileConnects(*this, position, myOut, rotateIndex(2, tile.rotation), 3);
        bool sideA  = tileConnects(*this, position, myOut, rotateIndex(1, tile.rotation), 3);
        bool sideB  = tileConnects(*this, position, myOut, rotateIndex(3, tile.rotation), 3);

        uint8_t inputs = (behind ? 1 : 0) + (sideA ? 1 : 0) + (sideB ? 1 : 0);
        uint8_t frame = 0;

        if      (inputs >= 3)    frame = 3;
        else if (inputs == 2)    frame = 2;
        else if (sideA || sideB) frame = 1;
        
        int frameRotation = tile.rotation;

        if (frame == 1) {
            if      (sideA && !sideB) frameRotation = (tile.rotation + 3) & 3;
            else if (!sideA && sideB) frameRotation = (tile.rotation + 2) & 3;
        } else if (frame == 2) {
            if      (!behind) frameRotation = (tile.rotation + 2) & 3;
            else if (!sideA)  frameRotation = (tile.rotation + 1) & 3;
            else if (!sideB)  frameRotation = (tile.rotation + 3) & 3;
        }

        TextContext text{std::to_string(sideA) + std::to_string(sideB) + std::to_string(behind), Math::toVector2i(Vector2f{renderContext.dst.x, renderContext.dst.y})};
        renderer.renderText(text);

        renderContext.src.x = frame * Game::TILE_SIZE_IN_PIXELS;
        renderContext.src.y = 0;

        renderContext.rotation = (frameRotation * 90);

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
            machine.update(deltaTime, *this, machine);
        }
    }
}
