#include <cstdint>
#include <string>

#include "Game.hpp"

#include "math/Math.hpp"
#include "math/Vectors.hpp"
#include "renderer/Context.hpp"
#include "world/objects/Tile.hpp"

#include "world/World.hpp"

World::World() {
    tile_settings.push_back(TileSetting{});

    tile_settings.push_back(TileSetting{
        .is_machine = true,
        .is_multi_tiled = true,
        .size_x = 2, .size_y = 2,
        .inventory_size = 2,
        .ios = {
            TileIO{TYPE::INPUT, SIDE::DOWN, 0, 1, 0}, 
            TileIO{TYPE::OUTPUT, SIDE::UP, 0, 0, 1}
        },
        .machine_type = "furnace"
    });

    tile_settings.push_back(TileSetting{});

    tile_settings.push_back(TileSetting{
        .is_machine = true,
        .inventory_size = 1,
        .ios = {},
        .machine_type = "conveyor"
    });

    tile_settings.push_back(TileSetting{
        .is_machine = true,
        .inventory_size = 2,
        .ios = {
            {
                .type = TYPE::OUTPUT, 
                .side = SIDE::DOWN, 
                .slot = 0
            },
            {
                .type = TYPE::OUTPUT, 
                .side = SIDE::UP, 
                .slot = 0
            },
            {
                .type = TYPE::OUTPUT, 
                .side = SIDE::LEFT, 
                .slot = 0
            },
            {
                .type = TYPE::OUTPUT, 
                .side = SIDE::RIGHT, 
                .slot = 0
            }
        },
        .machine_type = "drill"
    });

    recipe_manager.addRecipe(Recipe{}.add({1}).setOutput({2}));

    machine_logic[""] = {[](double deltaTime, World& world, Machine& machine) {}};

    machine_logic["furnace"] = {
        [](double deltaTime, World& world, Machine& machine) {
            Resource& extractSlot = machine.slots.front();

            machine.ticks += deltaTime;
            if (machine.ticks <= 1.0) return;
            machine.ticks = 0;

            MachineIO::insert(world, machine, 1);

            if (extractSlot.amount <= 0) {
                return;
            }

            Recipe recipeInput{};
            Resource& resource = world.recipeManager().getRecipeOutput(recipeInput.add({extractSlot.item_id}));

            Resource& outputSlot = machine.slots.back();

            if (outputSlot.canStack(resource)) {
                extractSlot.remove(1);
                outputSlot.add(resource.item_id, 1);
            } else {
                machine.ticks = 0;
            }
        }
    };

    machine_logic["conveyor"] = {
        [](double deltaTime, World& world, Machine& machine) {
            Resource& slot = machine.slots.front();
            slot.max_size = 1;
            if (slot.amount <= 0) {
                machine.ticks = 0;
                machine.active = false;
                return;
            }

            const Vector2i& offset = MachineIO::direction(machine.tile.rotation);
            const Vector2i nextPos = machine.position.add(offset);

            Machine* next = world.getMachine(nextPos);
            if (!next) {
                machine.ticks = 0;
                return;
            }

            Resource& nextSlot = next->slots.front();
            if (nextSlot.isFull()) {
                machine.ticks = 0;
                return;
            }

            machine.ticks += deltaTime * 2;
            if (machine.ticks <= 1.0) return;

            if (!MachineIO::insertResource(slot, nextSlot, 1)) {
                machine.ticks = 0;
            }
        }
    };

    machine_logic["drill"] = {
        [](double deltaTime, World& world, Machine& machine) {
            machine.ticks += deltaTime;
            if (machine.ticks < 1) return;
            machine.ticks = 0;

            int resourceID = 1;

            Resource& extractSlot = machine.slots.front();
            extractSlot.add(resourceID, 1);

            MachineIO::insert(world, machine, 1);
        }
    };
}

Tile* World::getTile(const Vector2i& position) {
    const auto& it = tiles.find(position);
    if (it == tiles.end())
        return nullptr;
    return &it->second;
}

Tile* World::getMainTile(Vector2i* position) {
    const Vector2i current{*position};
    Tile* tile = getTile(current);
    if (!tile) return nullptr;

    if (tile->isMultiTile()) {
        Vector2i mainPosition = tile->getMainPosition(current);
        position->x = mainPosition.x;
        position->y = mainPosition.y;
    }

    const auto& it = tiles.find(*position);
    if (it == tiles.end())
        return tile;

    return &it->second;
}

Machine* World::getMachine(const Vector2i& position) {
    Vector2i mainPosition = position;
    Tile* main = getMainTile(&mainPosition);
    if (!main) return nullptr;

    const auto& it = machines.find(mainPosition);
    if (it == machines.end()) 
        return nullptr;

    return &it->second;
}

const Machine* World::getConstMachine(const Vector2i& position) {
    return getMachine(position);
}

void World::addTile(const Vector2i& position, Tile tile) {
    const auto& main = tiles.find(position);
    if (main != tiles.end() && main->second.isSolid())
        return;

    const TileSetting& setting = getTileSetting(tile.id);
    Tile& current = tiles[position];

    if (setting.is_multi_tiled) {      
        for (int x = 0; x < setting.size_x; x++) {
            for (int y = 0; y < setting.size_y; y++) {
                if (x == 0 && y == 0) continue;
                Vector2i offsetPosition{position.x + x, position.y + y};
                const auto& it = tiles.find(offsetPosition);
                if (it != tiles.end() && !it->second.isAir())
                    return;
            }
        }

        for (int x = 0; x < setting.size_x; x++) {
            for (int y = 0; y < setting.size_y; y++) {
                if (x == 0 && y == 0) continue;
                Vector2i offsetPosition{position.x + x, position.y + y};
                Tile offsetTile{tile.id};
                offsetTile.setMainTile(position, offsetPosition);
                tiles[offsetPosition] = offsetTile;
            }
        }
    }

    current = std::move(tile);
    if (setting.is_machine) {
        const MachineLogic& machineType = machine_logic[setting.machine_type];
        Machine& machine = machines.emplace(position, Machine(current, setting.inventory_size)).first->second;
        machine.position = position;
        if (machineType.update != nullptr)
            machine.update = machineType.update;
    }
}

void World::removeTile(const Vector2i& position) {
    Vector2i main = position;
    const Tile* tile = getMainTile(&main);
    if (!tile) return;

    const TileSetting& setting = getTileSetting(tile->id);

    if (setting.is_machine) {
        machines.erase(main);
    }

    if (setting.is_multi_tiled) {
        for (int x = 0; x < setting.size_x; x++) {
            for (int y = 0; y < setting.size_y; y++) {
                tiles.erase({main.x + x, main.y + y});
            }
        }
    } else tiles.erase(main);
}

static int rotateIndex(int localIndex, int rot) {
    return (localIndex + rot) & 3;
}

bool World::tileConnects(Vector2i position, Vector2i myOut, int neighbourIndex, int tileId) {    
    const Vector2i& neighborDirection = MachineIO::direction(neighbourIndex);
    Vector2i tilePos = { position.x + neighborDirection.x,
                      position.y + neighborDirection.y };

    Tile* tile = getTile(tilePos);

    if (!tile) return false;
    const TileSetting& setting = getTileSetting(tile->id);
    if (setting.is_machine && tile->id != tileId) return true;

    const Vector2i& direction = MachineIO::direction(tile->rotation);
    Vector2i nOut = { tilePos.x + direction.x,
                      tilePos.y + direction.y };

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
        const Vector2i& direction = MachineIO::direction(tile.rotation);
        Vector2i myOut{ position.x + direction.x,
                        position.y + direction.y };

        bool behind = tileConnects(position, myOut, rotateIndex(2, tile.rotation), 3);
        bool sideA  = tileConnects(position, myOut, rotateIndex(1, tile.rotation), 3);
        bool sideB  = tileConnects(position, myOut, rotateIndex(3, tile.rotation), 3);

        uint8_t inputs = (behind ? 1 : 0) + (sideA ? 1 : 0) + (sideB ? 1 : 0);
        uint8_t frame = 0;

        if      (inputs >= 3)    frame = 3;
        else if (inputs == 2)    frame = 2;
        else if (sideA || sideB) frame = 1;
        
        int frameRotation = tile.rotation;

        if (frame == 1) {
            if      (sideA && !sideB) frameRotation = rotateIndex(tile.rotation, 3);
            else if (!sideA && sideB) frameRotation = rotateIndex(tile.rotation, 2);
        } else if (frame == 2) {
            if      (!behind) frameRotation = rotateIndex(tile.rotation, 2);
            else if (!sideA)  frameRotation = rotateIndex(tile.rotation, 1);
            else if (!sideB)  frameRotation = rotateIndex(tile.rotation, 3);
        }

        renderContext.src.x = frame * Game::TILE_SIZE_IN_PIXELS;
        renderContext.src.y = 0;

        renderContext.rotation = (frameRotation * 90);
    } else if (tile.offset > 0) {
        Vector2i tileOffset = tile.getOffset();
        blockTexture = Game::assetManager().getTexture(textures[tile.id]);
        
        renderContext.src.x = tileOffset.x * 16;
        renderContext.src.y = tileOffset.y * 16;
    }

    renderer.renderTexture(blockTexture, renderContext);
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

    Vector2f size{factor * 0.5f, factor * 0.5f};
    for (auto& [_, machine] : machines) {
        if (machine.tile.id != 3) continue;
        
        Vector2f render = Math::toVector2f(machine.position)
            .multiply(Game::TILE_SIZE_IN_PIXELS)
            .minus(cameraOffset)
            .multiply(renderer.zoom);

        Resource& slot = machine.slots.front();
        if (slot.amount > 0) {
            const Vector2f& dir = Math::toVector2f(MachineIO::direction(machine.tile.rotation));

            render += size.multiply(0.5f);
            render += size.multiply(2)
                        .multiply(dir.multiply(machine.ticks));

            renderer.renderTexture(dirt, render, size);
        }
    }
}   

void World::update(double deltaTime) {
    for (auto& [position, machine] : machines) {
        if (machine.update != nullptr) {
            machine.update(deltaTime, *this, machine);
        }
    }
}