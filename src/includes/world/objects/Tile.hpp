#pragma once

#include <cstdint>
#include <vector>
#include <functional>

#include "math/Vectors.hpp"
#include "world/storage/Inventory.hpp"

struct Tile {
    uint16_t id : 8 = 0;
    int16_t offset : 6 = 0;
    uint16_t rotation : 2 = 0;

    bool isAir() const {
        return !isSolid();
    }

    bool isSolid() const {
        return isMainTile() || isMultiTile();
    }

    bool isMainTile() const {
        return id != 0 && offset == 0;
    }

    bool isMultiTile() const {
        return id == 0 && offset != 0;
    }

    Vector2i getOffset() const {
        if (!isMultiTile())
            return {0, 0};
        
        int8_t x = offset & 0x7;
        int8_t y = (offset >> 3) & 0x7;

        if (x >= 4) x -= 8;
        if (y >= 4) y -= 8;

        return { x, y };
    }

    Vector2i getMainPosition(const Vector2i& pos) const {
        Vector2i tileOffset = getOffset();
        return { pos.x - tileOffset.x, pos.y - tileOffset.y };
    }

    void setMainTile(const Vector2i& main, const Vector2i& position) {
        int8_t dx = position.x - main.x;
        int8_t dy = position.y - main.y;
        offset = ((dy & 0x7) << 3) | (dx & 0x7);
    }
};

enum struct SIDE {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
};

enum struct TYPE {
    INPUT = 0,
    OUTPUT = 1
};

struct TileIO {
    TYPE type;
    SIDE side;
    uint8_t x = 0;
    uint8_t y = 0;
};

struct TileSetting {
    bool is_machine = false;
    bool is_multi_tiled = false;

    uint8_t size_x = 1;
    uint8_t size_y = 1;

    uint8_t inventory_size = 0;

    std::vector<TileIO> ios{};

    std::string machine_type = "";
};

class World;
class Machine;
struct MachineLogic {
    std::function<void(double, World&, Machine&)> update;
};